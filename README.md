Use Chrome to profile c/c++ code.



Usage: 1.ChromeUtilsStartTimings();



​				2.PROFILE("new_scene");

​					...

​					PROFILE("delete_scene");

​					...

​				3.ChromeUtilsStopTimingsAndWriteJsonFile("timings");



Open Chrome press F12,select the Performance tab and drag timings.txt into the center emplace.

