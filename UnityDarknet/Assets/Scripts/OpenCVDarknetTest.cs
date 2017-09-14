using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine;
using System;
using System.Threading;

public class OpenCVDarknetTest : MonoBehaviour
{

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void MyDelegate(string str);

    public int cacheTexCount = 5;
    private int cacheTexIdx = 0;
    private Texture2D[] cacheTex;
    private Texture2D tex;
    private byte[] imgData;
    private Color[] colorData;
    private int[] test;

    // Use this for initialization
    IEnumerator Start()
    {
        MyDelegate callback_delegate = new MyDelegate(CallBackFunction);
        // Convert callback_delegate into a function pointer that can be
        // used in unmanaged code.
        IntPtr intptr_delegate = Marshal.GetFunctionPointerForDelegate(callback_delegate);
        // Call the API passing along the function pointer.
        OpenCVInteropTest.SetDebugFunction(intptr_delegate);

        OpenCVInteropTest.Test("Hello World!");
        OpenCVInteropTest.Init("yolo-obj.cfg", "yolo-obj_5000.weights", "data/obj.names", ""); // ("yolo-voc.cfg", "yolo-voc.weights", "data/voc.names", "");
        OpenCVInteropTest.Open(1, 640, 480);
        cacheTex = new Texture2D[cacheTexCount];
        for(int i = 0; i < cacheTexCount; i++)
        {
            cacheTex[i] = new Texture2D(640, 480, TextureFormat.RGB24, false);
        }

        tex = new Texture2D(640, 480, TextureFormat.RGB24, false);

        imgData = new byte[640 * 480 * 3];
        colorData = new Color[640 * 480];
        test = new int[4];
        OpenCVInteropTest.TestArray(test);
        Debug.LogError("int list");
        new List<int>(test).ForEach(x => Debug.LogError(x));
        GetComponent<MeshRenderer>().material.mainTexture = tex;
        GetComponent<MeshRenderer>().material.SetTextureScale("_MainTex", new Vector2(-1, 1)); // flip texture horizontally, due to memcpy little endian

        //OpenCVInterop.Detect(imgData, "yolo-voc.cfg", "yolo-voc.weights", "data/voc.names", "camera", "", 0.200000003F, false);
        //Debug.LogError("data size: " + imgData.Length);

        //GCHandle handle = GCHandle.Alloc(imgData, GCHandleType.Pinned);
        //IntPtr ptr = handle.AddrOfPinnedObject();
        //Marshal.Copy(imgData, 0, ptr, 640 * 480 * 3);

        //var incoming = new byte[1280 * 720 * 3];
        //unsafe
        //{
        //    fixed (byte* inBuf = incoming)
        //    {
        //        byte* outBuf = OpenCVInterop.Detect("yolo-voc.cfg", "yolo-voc.weights", "data/voc.names", "camera", 1280, 720);
        //        // Assume, that the same buffer is returned, only with data changed.
        //        // Or by any other means, get the real length of output buffer (e.g. from library docs, etc).
        //        for (int i = 0; i < incoming.Length; i++)
        //            incoming[i] = outBuf[i];
        //    }
        //}
        yield return StartCoroutine(UpdateFrame());
    }

    // Update is called once per frame
    IEnumerator UpdateFrame()
    {
        while (true)
        {
            yield return new WaitForEndOfFrame();
            //OpenCVInterop.TestShow();

            //imgData = OpenCVInterop.Detect("yolo-voc.cfg", "yolo-voc.weights", "data/voc.names", "camera", "", 0.200000003F, false);
            ////Texture2D tex = new Texture2D(640, 480, TextureFormat.RGB24, false);
            //tex.LoadRawTextureData(imgData);
            //GetComponent<MeshRenderer>().material.mainTexture = tex;
            //GetComponent<MeshRenderer>().material.SetTextureScale("_MainTex", new Vector2(-1, 1)); // flip texture horizontally, due to memcpy little endian
            //tex.Apply();

            //Debug.LogError(imgData.Length);

            //IntPtr ptr = OpenCVInterop.Detect("yolo-voc.cfg", "yolo-voc.weights", "data/voc.names", "camera", "", 0.200000003F, false);
            //Marshal.Copy(ptr, imgData, 0, 640 * 480 * 3);

            OpenCVInteropTest.Detect(imgData, "camera", 0.200000003F, false);
            //for (var i = 0; i < imgData.Length; i += 3)
            //{
            //    var color = new Color(imgData[i + 0], imgData[i + 1], imgData[i + 2], 0);
            //    colorData[i / 3] = color;
            //}
            //tex.SetPixels(colorData);

            //tex = cacheTex[cacheTexIdx];
            tex.LoadRawTextureData(imgData);
            tex.Apply();
            cacheTexIdx = (cacheTexIdx + 1) % cacheTexCount;
        }

        //unsafe
        //{
        //    byte* ptr = (byte*)OpenCVInterop.Detect("yolo-voc.cfg", "yolo-voc.weights", "data/voc.names", "camera", "", 0.200000003F, false);

        //    for (var i = 0; i < 640*480*3; i += 3)
        //    {
        //        var color = new Color((float)ptr[i + 0], (float)ptr[i + 1], (float)ptr[i + 2], 255f);
        //        colorData[i / 3] = color;
        //    }
        //    tex.SetPixels(colorData);

        //    //int offset = 0;
        //    //for (int i = 0; i < 640; i++)
        //    //{
        //    //    for (int j = 0; j < 480; j++)
        //    //    {

        //    //        float b = (float)ptr[offset + 0] / 255.0f;
        //    //        float g = (float)ptr[offset + 1] / 255.0f;
        //    //        float r = (float)ptr[offset + 2] / 255.0f;
        //    //        float a = 255.0f;
        //    //        offset += 4;

        //    //        UnityEngine.Color color = new UnityEngine.Color(r, g, b, a);
        //    //        tex.SetPixel(j, 480 - i, color);
        //    //    }
        //    //}
        //}

        //StartCoroutine(UpdateTexture(imgData));
        //Debug.Log(imgData.Length);
        //for (var i = 0; i < imgData.Length; i += 3)
        //{
        //    var color = new Color(imgData[i + 0], imgData[i + 1], imgData[i + 2], 255);
        //    colorData[i / 3] = color;
        //}
        //tex.SetPixels(colorData);
    }

    void UpdateTexture(byte[] imgData)
    {
        //Texture2D tex = new Texture2D(640, 480, TextureFormat.RGB24, false);
        //tex.LoadRawTextureData(imgData);

        var colorArray = new Color32[imgData.Length / 3];

        for (var i = 0; i < imgData.Length; i++)
        {
            var color = new Color32(imgData[i + 0], imgData[i + 1], imgData[i + 2], 255);
            colorArray[i / 3] = color;
        }
        tex.SetPixels32(colorArray);

        GetComponent<MeshRenderer>().material.mainTexture = tex;
        GetComponent<MeshRenderer>().material.SetTextureScale("_MainTex", new Vector2(-1, 1)); // flip texture horizontally, due to memcpy little endian
        tex.Apply();
        //yield return null;
    }

    private void OnApplicationQuit()
    {
        OpenCVInteropTest.Close();
    }

    static void CallBackFunction(string str)
    {
        Debug.Log("::Native DLL callback : " + str);
    }
}

// Define the functions which can be called from the .dll.
internal static class OpenCVInteropTest
{
    [DllImport("yolo_console_dll")]
    internal static extern void Test(string s);

    [DllImport("yolo_console_dll")]
    internal static extern void Init(string cfg, string weights, string names, string file);

    [DllImport("yolo_console_dll")]
    internal static extern int Open(int camIdx, int width, int height);

    [DllImport("yolo_console_dll")]
    internal static extern void TestShow();

    [DllImport("yolo_console_dll")]
    internal static extern void Close();

    [DllImport("yolo_console_dll")]
    internal static extern void SetDebugFunction(IntPtr fp);

    [DllImport("yolo_console_dll")]
    internal static extern bool Detect([In, Out] byte[] data, string type, float thresh, bool useMean);

    [DllImport("yolo_console_dll")]
    internal static extern bool TestArray([In, Out] int[] array);
}
