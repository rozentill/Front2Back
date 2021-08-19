import xml.etree.ElementTree as etree
import xml.dom.minidom

import subprocess

import numpy as np
import os
import cv2

def main_render():
    rotations = np.linspace(
        start=0.0,
        stop=360.0,
        num=75,
        endpoint=False
    )

    for (frame, rotation) in enumerate(rotations, 1):

        scene = etree.Element("scene", version="0.5.0")

        sensor = etree.SubElement(
            scene,
            "sensor",
            type="perspective"
        )

        sensor_transform = etree.SubElement(
            sensor,
            "transform",
            name="toWorld"
        )

        etree.SubElement(
            sensor_transform,
            "lookat",
            origin="0, 0, 2.2",
            target="0, 0, 0",
            up="0, 1, 0"
        )

        sensor_sampler = etree.SubElement(
            sensor,
            "sampler",
            type="ldsampler"
        )

        etree.SubElement(
            sensor_sampler,
            "integer",
            name="sampleCount",
            value="128"
        )

        etree.SubElement(
            scene,
            "emitter ",
            type="constant",
            id="env"
        )


        sensor_film = etree.SubElement(
            sensor,
            "film",
            type="ldrfilm"
        )

        etree.SubElement(
            sensor_film,
            "boolean",
            name="banner",
            value="false"
        )

        etree.SubElement(
            sensor_film,
            "integer",
            name="width",
            value="256"
        )

        etree.SubElement(
            sensor_film,
            "integer",
            name="height",
            value="256"
        )

        cube = etree.SubElement(
            scene,
            "shape",
            type="obj"
        )

        obj = etree.SubElement(
            cube,
            "string",
            name="filename",
            value="D:\\Project\\front2back_paper\\video\\gt\\e6ea5e70c2f29d881e8fd793667dc14f.obj"
            # value="D:\\Data\\ShapeNet_test\\rot_gt\\03001627\\11506b96d41f7d3dd7c4a943f33e0384.obj"
            # value="D:\\Project\\3DFront2Back\\test\\obj\\11506b96d41f7d3dd7c4a943f33e0384_dirichlet.obj"
        )

        etree.SubElement(
            cube,
            "float",
            name="maxSmoothAngle",
            value="60"
        )
        
        outbsdf = etree.SubElement(
            cube,
            "bsdf",
            type="twosided"
            )
        bsdf = etree.SubElement(
            outbsdf,
            "bsdf",
            type="diffuse"
            )
        etree.SubElement(
            bsdf,
            "srgb",
            name="diffuseReflectance",
            value="0.76, 0.62, 0.37"
            )
        # etree.SubElement(
        #     bsdf,
        #     "string",
        #     name="distribution",
        #     value="ggx"
        #     )
        # etree.SubElement(
        #     bsdf,
        #     "float",
        #     name="alpha",
        #     value="0.1"
        #     )
        # etree.SubElement(
        #     bsdf,
        #     "float",
        #     name="intIOR",
        #     value="1.49"
        #     )
        cube_transform = etree.SubElement(
            cube,
            "transform",
            name="toWorld"
        )

        etree.SubElement(
            cube_transform,
            "scale",
            value="1.0"
        )

        etree.SubElement(
            cube_transform,
            "rotate",
            angle="{angle:.12f}".format(angle=rotation),
            y="1"
        )

        # etree.SubElement(
        #     cube_transform,
        #     "translate",
        #     # angle="{angle:.12f}".format(angle=rotation),
        #     z="-1.646"
        # )

        rough_string = etree.tostring(scene, "utf-8")
        reparsed = xml.dom.minidom.parseString(rough_string)

        reparsed_pretty = reparsed.toprettyxml(indent=" " * 4)

        with open("cube_python.xml", "w") as cube_xml:
            cube_xml.write(reparsed_pretty)

        # cmd = [
        #     "D:\\Develop\\Mitsuba 0.5.0 64bit\\Mitsuba 0.5.0\\mitsuba.exe",
        #     "-o", "test\\cube_python_{n:02d}.png".format(n=frame),
        #     "cube_python.xml"
        # ]
        # print(cmd)
        # cmd_out = subprocess.check_output(cmd)
        cmd = '"D:\\Tool\\Mitsuba 0.5.0 64bit\\Mitsuba 0.5.0\\mitsuba.exe" -o test\\turntable-gt\\cube_%02d.png cube_python.xml'%frame
        os.system(cmd)


def main_video():


    image_folder = 'test/turntable-our'
    video_name = 'test/turntable-our3.gif'

    # images = [img for img in os.listdir(image_folder) if img.endswith(".png")]
    # frame = cv2.imread(os.path.join(image_folder, images[0]))
    # height, width, layers = frame.shape

    # video = cv2.VideoWriter(video_name, 0, 15, (width,height))

    # for image in images:
    #     video.write(cv2.imread(os.path.join(image_folder, image)))

    # cv2.destroyAllWindows()
    # video.release()

    import imageio
    images = []
    filenames = os.listdir(image_folder)
    for filename in filenames:
        images.append(imageio.imread(os.path.join(image_folder, filename)))
    imageio.mimsave(video_name, images,  duration = 0.06)

if __name__ == '__main__':
    main_render()