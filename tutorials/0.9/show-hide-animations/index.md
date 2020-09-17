---
layout: page
title: Show/Hide animations
breadcrumb: animations
---

<img src="/resources/Tutorials/ShowHideAnimations.gif" alt="Show/Hide Animations" width="320" height="227" />

Do you want your widget to fly in from the side or fade out? TGUI supports these animations out of the box!

All you have to do is tell the showWithEffect or hideWithEffect function what type of animation you want and how long the animation should last.
```c++
button1->showWithEffect(tgui::ShowAnimationType::Fade, 800);
button2->showWithEffect(tgui::ShowAnimationType::SlideFromLeft, 500);
button3->hideWithEffect(tgui::ShowAnimationType::SlideToBottom, 500);
```

When the second parameter of the function call is an integer then it represents the time in milliseconds. You can alternatively pass an object of type `std::chrono::duration`, which is e.g. returned by `std::chrono::seconds(2)`.
