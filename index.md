---
layout: page
title: Texus' Graphical User Interface
redirect_from: "/v0.7-dev/index.html"
---

TGUI is an easy to use cross-platform c++ GUI library for [SFML](https://www.sfml-dev.org).

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
    <h3>Cross-platform</h3>
    <p>TGUI will work on all platforms that SFML supports. This means that you can use it on <b>Windows</b>, <b>Linux</b>, <b>Mac OS X</b>, <b>FreeBSD</b> and experimentally on <b>Raspberry Pi</b>, <b>Android</b> and <b>iOS</b>.</p>
  </div>
  <div class="HomePageSmallerColumn">
    <img src="/resources/CrossPlatform.jpg" alt="Cross Platform" width="340" height="185" />
  </div>
</div>


<div class="HomePageSmallerColumn">
  <img src="/resources/UniqueCallbackSystem.png" alt="Unique Callback System" width="364" height="195" />
</div>
<div class="HomePageLargerColumn">
  <h3>Unique Callback System</h3>
  <p>The callback system was written specifically for TGUI and aims to be as easy as possible. You will not have to call std::bind yourself when passing parameters and you can leave parameters unbound which the widgets can use to pass information.</p>
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
