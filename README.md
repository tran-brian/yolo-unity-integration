# yolo-unity-integration

## Integration of Darknet YOLO for use in Unity, by passing annotated webcam footage from OpenCV to Unity through P/Invoke.

### Notes:
* __Neither project will run without a compiled opencv_world{version}.dll__
* opencv_world dll should be placed in the following directories:
	* "Assets/Plugins" directory of the Unity project
	* "build/darknet/x64" directory of the Darknet project
* Custom code for the Darknet project can primarily be found in the "yolo_console_dll.cpp" file found in the "source" directory
