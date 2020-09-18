---
layout: page
title: Texus' Graphical User Interface
redirect_from: "/v0.7-dev/index.html"
---

TGUI is a cross-platform modern c++ GUI library.  
Although TGUI has always been a library for [SFML](https://www.sfml-dev.org), since TGUI 0.9 you can use custom backends and an experimental backend is provided that uses SDL2 with OpenGL 4.

A .Net binding for the library is available at [tgui.net](https://tgui.net) (only for TGUI 0.8).

<h3>Easy and customizable</h3>
The gui is easy to use, with only a few lines you can e.g. have a fully functional TextBox on your screen. The widgets can be created by just using colors or by using images, making the look very customizable.

<div>
  <a href="/resources/Screenshots/White.jpg" onclick="return showLightBox(event, href);"><img src="/resources/Screenshots/White-small.jpg" alt="White theme" width="246" height="185"/></a>
  <a href="/resources/Screenshots/Black.jpg" onclick="return showLightBox(event, href);"><img src="/resources/Screenshots/Black-small.jpg" alt="Black theme" width="246" height="185"/></a>
  <a href="/resources/Screenshots/BabyBlue.jpg" onclick="return showLightBox(event, href);"><img src="/resources/Screenshots/BabyBlue-small.jpg" alt="BabyBlue theme" width="246" height="185"/></a>
  <a href="/resources/Screenshots/KronosGame.jpg" onclick="return showLightBox(event, href);"><img src="/resources/Screenshots/KronosGame-small.jpg" alt="Kronos Game theme" width="246" height="185"/></a>
</div>

<div>
  <div class="HomePageLargerColumn">
    <h3>Gui Builder</h3>
    <p>TGUI comes with its own Gui Builder, which allows designing your gui more easily. The widgets are loaded in your program by simple calling gui.loadWidgetsFromFile("form.txt").</p>
  </div>
  <div class="HomePageSmallerColumn">
    <a href="/resources/GuiBuilder-0.8.5.png" onclick="return showLightBox(event, href);"><img src="/resources/GuiBuilder-0.8.5-small.jpg" alt="Gui Builder" width="360" height="195" /></a>
  </div>
</div>

<div>
  <div class="HomePageSmallerColumn">
    <img src="/resources/CrossPlatform.jpg" alt="Cross Platform" width="340" height="185" />
  </div>
  <div class="HomePageLargerColumn">
    <h3>Cross-platform</h3>
    <p>TGUI will work on all platforms that SFML supports. This means that you can use it on <b>Windows</b>, <b>Linux</b>, <b>macOS</b>, <b>FreeBSD</b> and experimentally on <b>Raspberry Pi</b>, <b>Android</b> and <b>iOS</b>.</p>
  </div>
</div>


<!-- Make some of the images use a lightbox when javascript is enabled -->
<script type="text/javascript">
    function showLightBox(event, href) {
        if (event.ctrlKey || event.shiftKey) {
            return true;
        }

        var background = document.createElement("div");
        background.id = "LightBox";
        background.onclick = function() { hideLightBox(); }
        document.getElementById("contents").appendChild(background);

        var image = document.createElement("img");
        image.src = href;
        background.appendChild(image);
        return false;
    }

    function hideLightBox() {
        var lightbox = document.getElementById("LightBox");
        if (lightbox) {
            document.getElementById("contents").removeChild(lightbox);
        }
    }
</script>
