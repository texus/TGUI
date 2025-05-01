/*
 *  raymob License (MIT)
 *
 *  Copyright (c) 2023-2024 Le Juez Victor
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */

package com.raylib.raymob;

import android.view.inputmethod.InputMethodManager;
import android.app.NativeActivity;
import android.content.Context;
import android.view.KeyEvent;

public class SoftKeyboard {

    private final Context context;
    private final InputMethodManager imm;
    private KeyEvent lastKeyEvent = null;

    public SoftKeyboard(Context context) {
        imm = (InputMethodManager)context.getSystemService(Context.INPUT_METHOD_SERVICE);
        this.context = context;
    }

    /* PUBLIC FOR JNI (raymob.h) */

    public void showKeyboard() {
        imm.showSoftInput(((NativeActivity)context).getWindow().getDecorView(), InputMethodManager.SHOW_FORCED);
    }

    public void hideKeyboard() {
        imm.hideSoftInputFromWindow(((NativeActivity)context).getWindow().getDecorView().getWindowToken(), 0);
    }

    public int getLastKeyCode() {
        if (lastKeyEvent != null) return lastKeyEvent.getKeyCode();
        return 0;
    }

    public char getLastKeyLabel() {
        if (lastKeyEvent != null) return lastKeyEvent.getDisplayLabel();
        return '\0';
    }

    public int getLastKeyUnicode() {
        if (lastKeyEvent != null) return lastKeyEvent.getUnicodeChar();
        return 0;
    }

    public void clearLastEvent() {
        lastKeyEvent = null;
    }

    /* PRIVATE FOR JNI (raymob.h) */

    public void onKeyUpEvent(KeyEvent event) {
        lastKeyEvent = event;
    }
}
