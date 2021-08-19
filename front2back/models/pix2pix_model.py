import numpy as np
import torch
import torch.nn.functional as F
import os
from collections import OrderedDict
from torch.autograd import Variable
import util.util as util
from util.image_pool import ImagePool
from .base_model import BaseModel
from . import networks
import math

class Pix2PixModel(BaseModel):
    def name(self):
        return 'Pix2PixModel'

    def initialize(self, opt):
        BaseModel.initialize(self, opt)
        self.isTrain = opt.isTrain
        self.batchSize = opt.batchSize
        # define tensors
        self.input_A = self.Tensor(opt.batchSize, opt.input_nc,
                                   opt.fineSize, opt.fineSize)
        self.input_B = self.Tensor(opt.batchSize, opt.output_nc,
                                   opt.fineSize, opt.fineSize)

        # load/define networks
        self.netG = networks.define_G(opt.input_nc, opt.output_nc, opt.ngf,
                                      opt.which_model_netG, opt.norm, not opt.no_dropout, self.gpu_ids)
        if self.isTrain:
            use_sigmoid = opt.no_lsgan
            self.netD = networks.define_D(opt.input_nc + opt.output_nc, opt.ndf,
                                          opt.which_model_netD,
                                          opt.n_layers_D, opt.norm, use_sigmoid, self.gpu_ids)
        if not self.isTrain or opt.continue_train:
            self.load_network(self.netG, 'G', opt.which_epoch)
            if self.isTrain:
                self.load_network(self.netD, 'D', opt.which_epoch)

        if self.isTrain:
            self.fake_AB_pool = ImagePool(opt.pool_size)
            self.old_lr = opt.lr
            # define loss functions
            self.criterionGAN = networks.GANLoss(use_lsgan=not opt.no_lsgan, tensor=self.Tensor)
            self.criterionL1 = torch.nn.L1Loss()
            self.criterionL2 = torch.nn.MSELoss()
            self.criterionCOS = torch.nn.CosineSimilarity(dim = 1)
            # initialize optimizers
            self.optimizer_G = torch.optim.Adam(self.netG.parameters(),
                                                lr=opt.lr, betas=(opt.beta1, 0.999))
            self.optimizer_D = torch.optim.Adam(self.netD.parameters(),
                                                lr=opt.lr, betas=(opt.beta1, 0.999))
        else:
            self.criterionGAN = networks.GANLoss(use_lsgan=not opt.no_lsgan, tensor=self.Tensor)
            self.criterionL1 = torch.nn.L1Loss()
            self.criterionL2 = torch.nn.MSELoss()
            self.criterionCOS = torch.nn.CosineSimilarity(dim = 1)
        print('---------- Networks initialized -------------')
        networks.print_network(self.netG)
        if self.isTrain:
            networks.print_network(self.netD)
        print('-----------------------------------------------')

    def set_input(self, input):
        AtoB = self.opt.which_direction == 'AtoB'
        if self.opt.input_nc == 4:
            input_A = torch.cat((input['A' if AtoB else 'B'], input['A_depth' if AtoB else 'B_depth']),1)
            input_B = torch.cat((input['B' if AtoB else 'A'], input['B_depth' if AtoB else 'A_depth']),1)
        elif self.opt.input_nc == 5:
            input_A = torch.cat((input['A' if AtoB else 'B'], input['A_depth' if AtoB else 'B_depth'], input['A_edge' if AtoB else 'B_edge']), 1)
            input_B = torch.cat((input['B' if AtoB else 'A'], input['B_depth' if AtoB else 'A_depth'], input['B_edge' if AtoB else 'A_edge']), 1)
        elif self.opt.input_nc == 3:
            if self.opt.output_nc == 3:
                input_A = input['A' if AtoB else 'B']
                input_B = input['B' if AtoB else 'A']
            elif self.opt.output_nc == 4:
                input_A = input['A' if AtoB else 'B']
                input_B = torch.cat((input['B' if AtoB else 'A_normal'], input['B_depth' if AtoB else 'A_depth']), 1)
            elif self.opt.output_nc == 5:
                input_A = input['A' if AtoB else 'B']
                input_B = torch.cat((input['B' if AtoB else 'A_normal'], input['B_depth' if AtoB else 'A_depth'], input['B_edge' if AtoB else 'A_edge']), 1)
        elif self.opt.input_nc == 8:
            input_A = torch.cat((input['A' if AtoB else 'B'], input['A_depth' if AtoB else 'B_depth'], input['B_back_normal'], input['B_back_depth']),1)
            input_B = torch.cat((input['B' if AtoB else 'A'], input['B_depth' if AtoB else 'A_depth']),1)


        self.input_A.resize_(input_A.size()).copy_(input_A)
        self.input_B.resize_(input_B.size()).copy_(input_B)
        self.image_paths = input['A_paths' if AtoB else 'B_paths']

    def forward(self):
        self.real_A = Variable(self.input_A)
        self.fake_B = self.netG.forward(self.real_A)
        self.real_B = Variable(self.input_B)

    # no backprop gradients
    def test(self):
        self.real_A = Variable(self.input_A, volatile=True)
        self.fake_B = self.netG.forward(self.real_A)
        self.real_B = Variable(self.input_B, volatile=True)

    # get image paths
    def get_image_paths(self):
        return self.image_paths

    def count_active(self, image):
        active = 0
        for i in range(0, 256):
            for j in range(0, 256):
                if image[0][i][j] > 0 and image[1][i][j] > 0 and image[2][i][j] > 0:
                    active += 1
        # print("Now compute a number of active pixels :"+str(active)+" .\n")
        return active

    def backward_D(self):
        # Fake
        # stop backprop to the generator by detaching fake_B
        fake_AB = self.fake_AB_pool.query(torch.cat((self.real_A, self.fake_B), 1))
        self.pred_fake = self.netD.forward(fake_AB.detach())
        self.loss_D_fake = self.criterionGAN(self.pred_fake, False)

        # Real
        real_AB = torch.cat((self.real_A, self.real_B), 1)
        self.pred_real = self.netD.forward(real_AB)
        self.loss_D_real = self.criterionGAN(self.pred_real, True)

        # Combined loss
        self.loss_D = (self.loss_D_fake + self.loss_D_real) * 0.5

        self.loss_D.backward()

    def backward_G(self):
        # First, G(A) should fake the discriminator
        fake_AB = torch.cat((self.real_A, self.fake_B), 1)
        
		pred_fake = self.netD.forward(fake_AB)
        
		self.loss_G_GAN = self.criterionGAN(pred_fake, True) 
        
		self.loss_G_L1_depth = self.criterionL1(self.fake_B[:, 3:4, :, :], self.real_B[:, 3:4, :, :]) * 100 * 10

        self.loss_G_L1 =  self.loss_G_L1_depth

        self.loss_G_COS_normal = self.criterionCOS(self.fake_B[:,0:3,:,:], self.real_B[:,0:3,:,:])

        self.loss_G_COS_normal_size = 1

        for i in range(0, self.loss_G_COS_normal.dim()):
            self.loss_G_COS_normal_size *= self.loss_G_COS_normal.size(i)

        self.loss_G_COS_normal = (1-torch.sum(self.loss_G_COS_normal)/(self.loss_G_COS_normal_size))*100

        self.loss_G = self.loss_G_GAN + self.loss_G_L1 + self.loss_G_COS_normal 


        self.loss_G.backward()

    def optimize_parameters(self):
        self.forward()

        self.optimizer_D.zero_grad()
        self.backward_D()
        self.optimizer_D.step()

        self.optimizer_G.zero_grad()
        self.backward_G()
        self.optimizer_G.step()

    def optimize_parameters_D(self):
        self.forward()

        self.optimizer_D.zero_grad()
        self.backward_D()
        self.optimizer_D.step()

        self.optimizer_G.zero_grad()
        self.backward_G()
        self.optimizer_G.step()

    def optimize_parameters_G(self):
        self.forward()

        self.optimizer_G.zero_grad()
        self.backward_G()
        self.optimizer_G.step()

    def get_current_errors(self):
        return OrderedDict([
                            ('G_GAN', self.loss_G_GAN.data[0]),
                            ('G_L1_depth', self.loss_G_L1_depth.data[0]),
                            ('G_COS_noraml', self.loss_G_COS_normal.data[0]),
                            ('D_real', self.loss_D_real.data[0]),
                            ('D_fake', self.loss_D_fake.data[0])
                            ])

    def get_current_visuals(self):

        real_A = util.tensor2im(self.real_A.data[:,0:3,:,:])
        fake_B = util.tensor2im(self.fake_B.data[:,0:3,:,:])
        real_B = util.tensor2im(self.real_B.data[:,0:3,:,:])
        if self.opt.input_nc != 3:

            if  self.opt.output_nc == 6:
                real_A_depth = util.tensor2im(self.real_A.data[:, 3:6, :, :])
                fake_B_depth = util.tensor2im(self.fake_B.data[:, 3:6, :, :])
                real_B_depth = util.tensor2im(self.real_B.data[:, 3:6, :, :])
            elif self.opt.output_nc == 4:
                self.real_A_depth = torch.cat((self.real_A[:,3:4,:,:], self.real_A[:,3:4,:,:], self.real_A[:,3:4,:,:]), 1)
                self.fake_B_depth = torch.cat((self.fake_B[:,3:4,:,:], self.fake_B[:,3:4,:,:], self.fake_B[:,3:4,:,:]), 1)
                self.real_B_depth = torch.cat((self.real_B[:,3:4,:,:], self.real_B[:,3:4,:,:], self.real_B[:,3:4,:,:]), 1)
                real_A_depth = util.tensor2im(self.real_A_depth.data)
                fake_B_depth = util.tensor2im(self.fake_B_depth.data)
                real_B_depth = util.tensor2im(self.real_B_depth.data)
                return OrderedDict(
                    [('real_A', real_A), ('fake_B', fake_B), ('real_B', real_B), ('real_A_depth', real_A_depth),
                     ('fake_B_depth', fake_B_depth), ('real_B_depth', real_B_depth)])

            elif self.opt.output_nc == 5:

                self.fake_B_depth = torch.cat(
                    (self.fake_B[:, 3:4, :, :], self.fake_B[:, 3:4, :, :], self.fake_B[:, 3:4, :, :]), 1)
                self.real_B_depth = torch.cat(
                    (self.real_B[:, 3:4, :, :], self.real_B[:, 3:4, :, :], self.real_B[:, 3:4, :, :]), 1)
                fake_B_depth = util.tensor2im(self.fake_B_depth.data)
                real_B_depth = util.tensor2im(self.real_B_depth.data)


                self.fake_B_edge = torch.cat(
                    (self.fake_B[:, 4:5, :, :], self.fake_B[:, 4:5, :, :], self.fake_B[:, 4:5, :, :]), 1)
                self.real_B_edge = torch.cat(
                    (self.real_B[:, 4:5, :, :], self.real_B[:, 4:5, :, :], self.real_B[:, 4:5, :, :]), 1)
                fake_B_edge = util.tensor2im(self.fake_B_edge.data)
                real_B_edge = util.tensor2im(self.real_B_edge.data)
                return OrderedDict(
                    [('real_A', real_A), ('fake_B', fake_B), ('real_B', real_B), 
                     ('fake_B_depth', fake_B_depth), ('real_B_depth', real_B_depth), 
                     ('fake_B_edge', fake_B_edge), ('real_B_edge', real_B_edge)])


        else:
            if self.opt.output_nc == 5:

                self.fake_B_depth = torch.cat(
                    (self.fake_B[:, 3:4, :, :], self.fake_B[:, 3:4, :, :], self.fake_B[:, 3:4, :, :]), 1)
                self.real_B_depth = torch.cat(
                    (self.real_B[:, 3:4, :, :], self.real_B[:, 3:4, :, :], self.real_B[:, 3:4, :, :]), 1)
                fake_B_depth = util.tensor2im(self.fake_B_depth.data)
                real_B_depth = util.tensor2im(self.real_B_depth.data)


                self.fake_B_edge = torch.cat(
                    (self.fake_B[:, 4:5, :, :], self.fake_B[:, 4:5, :, :], self.fake_B[:, 4:5, :, :]), 1)
                self.real_B_edge = torch.cat(
                    (self.real_B[:, 4:5, :, :], self.real_B[:, 4:5, :, :], self.real_B[:, 4:5, :, :]), 1)
                fake_B_edge = util.tensor2im(self.fake_B_edge.data)
                real_B_edge = util.tensor2im(self.real_B_edge.data)
                return OrderedDict(
                    [('real_A', real_A), ('fake_B', fake_B), ('real_B', real_B), 
                     ('fake_B_depth', fake_B_depth), ('real_B_depth', real_B_depth), 
                     ('fake_B_edge', fake_B_edge), ('real_B_edge', real_B_edge)])
            elif self.opt.output_nc == 4:
                self.fake_B_depth = torch.cat(
                    (self.fake_B[:, 3:4, :, :], self.fake_B[:, 3:4, :, :], self.fake_B[:, 3:4, :, :]), 1)
                self.real_B_depth = torch.cat(
                    (self.real_B[:, 3:4, :, :], self.real_B[:, 3:4, :, :], self.real_B[:, 3:4, :, :]), 1)
                fake_B_depth = util.tensor2im(self.fake_B_depth.data)
                real_B_depth = util.tensor2im(self.real_B_depth.data)
                return OrderedDict(
                    [('real_A', real_A), ('fake_B', fake_B), ('real_B', real_B), 
                     ('fake_B_depth', fake_B_depth), ('real_B_depth', real_B_depth)
                     ])
            else:
                return OrderedDict([('real_A', real_A), ('fake_B', fake_B), ('real_B', real_B)])


    def save(self, label):
        self.save_network(self.netG, 'G', label, self.gpu_ids)
        self.save_network(self.netD, 'D', label, self.gpu_ids)

    def update_learning_rate(self):
        lrd = self.opt.lr / self.opt.niter_decay
        lr = self.old_lr - lrd
        for param_group in self.optimizer_D.param_groups:
            param_group['lr'] = lr
        for param_group in self.optimizer_G.param_groups:
            param_group['lr'] = lr
        print('update learning rate: %f -> %f' % (self.old_lr, lr))
        self.old_lr = lr
