// Top-level build file where you can add configuration options common to all sub-projects/modules.

buildscript {
    // AGP 8.x and older don't work with Gradle 9.6 or newer, while AGP 9.5 requires at least Gradle 9.6
    val androidGradlePluginVersion = if (GradleVersion.current().baseVersion >= GradleVersion.version("9.6")) "9.5.1" else "8.13.2"

    repositories {
        google()
        mavenCentral()
    }
    dependencies {
        classpath("com.android.tools.build:gradle:$androidGradlePluginVersion")
    }
}

allprojects {
    repositories {
        google()
        mavenCentral()
    }
}

tasks.withType<Wrapper> {
    gradleVersion = "9.5.1"
    distributionType = Wrapper.DistributionType.BIN
}

tasks.register("clean", Delete::class) {
    delete(rootProject.layout.buildDirectory)
}
