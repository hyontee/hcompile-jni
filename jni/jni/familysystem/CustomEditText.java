package com.blackhub.bronline.game.gui.familysystem;

import android.content.Context;
import android.util.AttributeSet;
import android.view.KeyEvent;
import androidx.appcompat.widget.AppCompatEditText;
import androidx.compose.runtime.internal.StabilityInferred;
import com.blackhub.bronline.game.GUIManager;
import kotlin.Metadata;
import kotlin.jvm.internal.Intrinsics;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;
import org.json.JSONObject;

/* JADX WARN: Classes with same name are omitted:
  C:\javanew\classes3.dex
 */
/* compiled from: CustomEditText.kt */
@StabilityInferred(parameters = 0)
@Metadata(d1 = {"\u0000<\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0002\b\u0002\n\u0002\u0010\b\n\u0000\n\u0002\u0018\u0002\n\u0000\n\u0002\u0010\u0002\n\u0002\b\u0002\n\u0002\u0010\u000b\n\u0002\b\u0002\n\u0002\u0018\u0002\n\u0002\b\u0006\b\u0007\u0018\u00002\u00020\u0001B\u0017\u0012\u0006\u0010\u0002\u001a\u00020\u0003\u0012\b\u0010\u0004\u001a\u0004\u0018\u00010\u0005¢\u0006\u0002\u0010\u0006J\u0010\u0010\u000b\u001a\u00020\f2\u0006\u0010\r\u001a\u00020\bH\u0016J\u001a\u0010\u000e\u001a\u00020\u000f2\u0006\u0010\u0010\u001a\u00020\b2\b\u0010\u0011\u001a\u0004\u0018\u00010\u0012H\u0016J\u0010\u0010\u0013\u001a\u00020\f2\u0006\u0010\u0014\u001a\u00020\bH\u0002J\b\u0010\u0015\u001a\u00020\bH\u0002J\u0010\u0010\u0016\u001a\u00020\f2\b\u0010\u0017\u001a\u0004\u0018\u00010\nR\u000e\u0010\u0007\u001a\u00020\bX\u0082\u000e¢\u0006\u0002\n\u0000R\u0010\u0010\t\u001a\u0004\u0018\u00010\nX\u0082\u000e¢\u0006\u0002\n\u0000¨\u0006\u0018"}, d2 = {"Lcom/blackhub/bronline/game/gui/familysystem/CustomEditText;", "Landroidx/appcompat/widget/AppCompatEditText;", "context", "Landroid/content/Context;", "attrs", "Landroid/util/AttributeSet;", "(Landroid/content/Context;Landroid/util/AttributeSet;)V", "counter", "", "guiManager", "Lcom/blackhub/bronline/game/GUIManager;", "onEditorAction", "", "actionCode", "onKeyPreIme", "", "keyCode", "event", "Landroid/view/KeyEvent;", "putValueTokenForGetCost", "valueOfToken", "sendToServerValueOfToken", "setGUIManager", "getGuiManager", "app_siteRelease"}, k = 1, mv = {1, 9, 0}, xi = 48)
/* loaded from: classes3.dex */
public final class CustomEditText extends AppCompatEditText {
    public static final int $stable = 8;
    public int counter;

    @Nullable
    public GUIManager guiManager;

    /* JADX WARN: 'super' call moved to the top of the method (can break code semantics) */
    public CustomEditText(@NotNull Context context, @Nullable AttributeSet attributeSet) {
        super(context, attributeSet);
        Intrinsics.checkNotNullParameter(context, "context");
    }

    public final void setGUIManager(@Nullable GUIManager getGuiManager) {
        this.guiManager = getGuiManager;
    }

    @Override // android.widget.TextView, android.view.View
    public boolean onKeyPreIme(int keyCode, @Nullable KeyEvent event) {
        if (keyCode != 4) {
            return false;
        }
        int i = this.counter + 1;
        this.counter = i;
        if (i % 2 == 0) {
            return false;
        }
        putValueTokenForGetCost(sendToServerValueOfToken());
        return false;
    }

    @Override // android.widget.TextView
    public void onEditorAction(int actionCode) {
        super.onEditorAction(actionCode);
        if (actionCode == 3 || actionCode == 4 || actionCode == 6) {
            putValueTokenForGetCost(sendToServerValueOfToken());
        }
    }

    public final int sendToServerValueOfToken() {
        String valueOf = String.valueOf(super.getText());
        Intrinsics.checkNotNull(valueOf);
        if (valueOf.length() <= 0 || valueOf.charAt(0) == '0') {
            return 0;
        }
        return Integer.parseInt(valueOf);
    }

    public final void putValueTokenForGetCost(int valueOfToken) {
        JSONObject jSONObject = new JSONObject();
        try {
            jSONObject.put("t", 9);
            jSONObject.put("s", 1);
            jSONObject.put("v", valueOfToken);
            GUIManager gUIManager = this.guiManager;
            if (gUIManager != null) {
                gUIManager.sendJsonData(45, jSONObject);
            }
        } catch (Exception unused) {
        }
    }
}
