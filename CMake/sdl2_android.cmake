set(PROJECT_ID "me.lejenome.mastermind")
set(android_project_path "${CMAKE_CURRENT_SOURCE_DIR}/ext/android")
configure_file("${android_project_path}/AndroidManifest.xml.in" AndroidManifest.xml)
configure_file("${android_project_path}/build.xml.in" build.xml)
configure_file(${android_project_path}/jni/src/Android.mk.in jni/src/Android.mk)
configure_file(${android_project_path}/res/values/strings.xml.in
	res/values/strings.xml)
configure_file(${android_project_path}/res/layout/main.xml.in res/layout/main.xml)
string(REPLACE "." "/" _src_path ${PROJECT_ID})
configure_file(${android_project_path}/main.java.in
	src/${_src_path}/${PROJECT_NAME}.java)

configure_file("${android_project_path}/ant.properties" . COPYONLY)
configure_file("${android_project_path}/build.properties" . COPYONLY)
configure_file("${android_project_path}/default.properties" . COPYONLY)
configure_file("${android_project_path}/project.properties" . COPYONLY)
configure_file("${android_project_path}/proguard-project.txt" . COPYONLY)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/extra/ProFont_r400-29.pcf"
	assets/share/fonts/ProFont_r400-29.pcf COPYONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/extra/fontawesome-webfont.ttf"
	assets/share/fonts/fontawesome-webfont.ttf COPYONLY)
configure_file(${android_project_path}/jni/src/Android_static.mk jni/src/ COPYONLY)
configure_file(${android_project_path}/src/org/libsdl/app/SDLActivity.java
	src/org/libsdl/app/SDLActivity.java COPYONLY)

configure_file(${android_project_path}/jni/Android.mk jni/ COPYONLY)
configure_file(${android_project_path}/jni/Application.mk jni/ COPYONLY)

configure_file(${android_project_path}/res/drawable-xxhdpi/ic_launcher.png
	res/drawable/ic_launcher.png COPYONLY)
