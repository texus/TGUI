plugins {
    id("com.android.application")
}

val NDK_VERSION by extra(project.properties["NDK_VERSION"] as? String ?: "28.0.13004108")
val ARCH_ABI by extra(project.properties["ARCH_ABI"] as? String ?: "arm64-v8a")

android {
    namespace = "eu.tgui.app"
    ndkVersion = NDK_VERSION
    compileSdk = 34
    defaultConfig {
        applicationId = "eu.tgui.app"
        minSdk = 23
        targetSdk = 34
        versionCode = 1
        versionName = "1.0"
        ndk {
            abiFilters.add(ARCH_ABI)
        }
        externalNativeBuild {
            cmake {
                arguments.add("-DANDROID_STL=c++_shared")
            }
        }
    }
    buildTypes {
        release {
            isMinifyEnabled = false
            proguardFiles(getDefaultProguardFile("proguard-android.txt"), "proguard-rules.pro")
        }
    }
    externalNativeBuild {
        cmake {
            path("src/main/jni/CMakeLists.txt")
            version = "3.22.1"
        }
    }
}

dependencies {
    implementation(fileTree(mapOf("dir" to "libs", "include" to listOf("*.jar"))))
}
