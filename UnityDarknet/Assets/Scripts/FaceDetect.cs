//Face detection using HAAR Cascade
using UnityEngine;
using System.IO;
using Emgu.CV;
using Emgu.CV.Structure;

public class FaceDetect : MonoBehaviour
{

    private int frameWidth;
    private int frameHeight;
    private VideoCapture cvCapture;
    private CascadeClassifier _cascadeClassifier;
    private Image<Bgr, byte> currentFrameBgr;

    public Material mt;

    void Start()
    {

        cvCapture = new VideoCapture(0);

        _cascadeClassifier = new CascadeClassifier(Application.dataPath + "/haarcascade_frontalface_alt.xml");

        frameWidth = (int)cvCapture.GetCaptureProperty(Emgu.CV.CvEnum.CapProp.FrameWidth);
        frameHeight = (int)cvCapture.GetCaptureProperty(Emgu.CV.CvEnum.CapProp.FrameHeight);

        cvCapture.Start();

    }

    void Update()
    {

        //FaceDetector();
    }


    private void FaceDetector()
    {
        currentFrameBgr = cvCapture.QueryFrame().ToImage<Bgr, byte>();

        Texture2D tex = new Texture2D(640, 480);

        if (currentFrameBgr != null)
        {
            Image<Gray, byte> grayFrame = currentFrameBgr.Convert<Gray, byte>();

            var faces = _cascadeClassifier.DetectMultiScale(grayFrame, 1.1, 4, new System.Drawing.Size(frameWidth / 8, frameHeight / 8));

            foreach (var face in faces)
            {
                currentFrameBgr.Draw(face, new Bgr(0, 255, 0), 3);
            }

            //Convert this image into Bitmap, the pixel values are copied over to the Bitmap
            currentFrameBgr.ToBitmap();

            MemoryStream memstream = new MemoryStream();
            currentFrameBgr.Bitmap.Save(memstream, currentFrameBgr.Bitmap.RawFormat);

            tex.LoadImage(memstream.ToArray());
            mt.mainTexture = tex;
        }
    }

    private void OnDestroy()
    {
        //release from memory
        cvCapture.Dispose();
        cvCapture.Stop();

    }
}
