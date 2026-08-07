package com.blackhub.bronline.game.gui.familysystem;

import android.text.Editable;
import android.view.LayoutInflater;
import android.view.View;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.widget.PopupWindow;
import android.widget.TextView;
import androidx.compose.runtime.internal.StabilityInferred;
import com.blackhub.bronline.R;
import com.blackhub.bronline.databinding.FamiliesBuyTokenDialogBinding;
import com.blackhub.bronline.game.core.JNIActivity;
import com.blackhub.bronline.game.gui.UsefulKt;
import com.blackhub.bronline.game.gui.familysystem.network.ActionsWithJSON;
import kotlin.Metadata;
import kotlin.jvm.internal.Intrinsics;
import kotlin.text.StringsKt__StringsJVMKt;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

/* compiled from: LogicForDialogBuyToken.kt */
@StabilityInferred(parameters = 0)
@Metadata(d1 = {"\u0000>\n\u0002\u0018\u0002\n\u0002\u0010\u0000\n\u0000\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0002\b\u0002\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0000\n\u0002\u0010\u0002\n\u0002\b\u0004\n\u0002\u0010\b\n\u0000\b\u0007\u0018\u00002\u00020\u0001B!\u0012\u0006\u0010\u0002\u001a\u00020\u0003\u0012\b\u0010\u0004\u001a\u0004\u0018\u00010\u0005\u0012\b\u0010\u0006\u001a\u0004\u0018\u00010\u0007¢\u0006\u0002\u0010\bJ\u0006\u0010\u000f\u001a\u00020\u0010J\u0006\u0010\u0011\u001a\u00020\u0010J\u0006\u0010\u0012\u001a\u00020\u0010J\u000e\u0010\u0013\u001a\u00020\u00102\u0006\u0010\u0014\u001a\u00020\u0015R\u0010\u0010\u0006\u001a\u0004\u0018\u00010\u0007X\u0082\u0004¢\u0006\u0002\n\u0000R\u0010\u0010\t\u001a\u0004\u0018\u00010\nX\u0082\u000e¢\u0006\u0002\n\u0000R\u0010\u0010\u000b\u001a\u0004\u0018\u00010\fX\u0082\u000e¢\u0006\u0002\n\u0000R\u0010\u0010\r\u001a\u0004\u0018\u00010\u000eX\u0082\u000e¢\u0006\u0002\n\u0000R\u0010\u0010\u0004\u001a\u0004\u0018\u00010\u0005X\u0082\u0004¢\u0006\u0002\n\u0000R\u000e\u0010\u0002\u001a\u00020\u0003X\u0082\u0004¢\u0006\u0002\n\u0000¨\u0006\u0016"}, d2 = {"Lcom/blackhub/bronline/game/gui/familysystem/LogicForDialogBuyToken;", "", "mainRoot", "Lcom/blackhub/bronline/game/gui/familysystem/GUIFamilySystem;", "mainActivity", "Lcom/blackhub/bronline/game/core/JNIActivity;", "actionsWithJson", "Lcom/blackhub/bronline/game/gui/familysystem/network/ActionsWithJSON;", "(Lcom/blackhub/bronline/game/gui/familysystem/GUIFamilySystem;Lcom/blackhub/bronline/game/core/JNIActivity;Lcom/blackhub/bronline/game/gui/familysystem/network/ActionsWithJSON;)V", "anim", "Landroid/view/animation/Animation;", "binding", "Lcom/blackhub/bronline/databinding/FamiliesBuyTokenDialogBinding;", "dialogBuyToken", "Landroid/widget/PopupWindow;", "closeDialog", "", "setNullableParameters", "showDialog", "updateCostInBC", "newCost", "", "app_siteRelease"}, k = 1, mv = {1, 9, 0}, xi = 48)
/* loaded from: classes3.dex */
public final class LogicForDialogBuyToken {
    public static final int $stable = 8;

    @Nullable
    public final ActionsWithJSON actionsWithJson;

    @Nullable
    public Animation anim;

    @Nullable
    public FamiliesBuyTokenDialogBinding binding;

    @Nullable
    public PopupWindow dialogBuyToken;

    @Nullable
    public final JNIActivity mainActivity;

    @NotNull
    public final GUIFamilySystem mainRoot;

    public LogicForDialogBuyToken(@NotNull GUIFamilySystem mainRoot, @Nullable JNIActivity jNIActivity, @Nullable ActionsWithJSON actionsWithJSON) {
        Intrinsics.checkNotNullParameter(mainRoot, "mainRoot");
        this.mainRoot = mainRoot;
        this.mainActivity = jNIActivity;
        this.actionsWithJson = actionsWithJSON;
        Object systemService = jNIActivity != null ? jNIActivity.getSystemService("layout_inflater") : null;
        Intrinsics.checkNotNull(systemService, "null cannot be cast to non-null type android.view.LayoutInflater");
        FamiliesBuyTokenDialogBinding inflate = FamiliesBuyTokenDialogBinding.inflate((LayoutInflater) systemService);
        this.binding = inflate;
        if (inflate != null) {
            this.anim = AnimationUtils.loadAnimation(inflate.getRoot().getContext(), R.anim.button_click);
            PopupWindow popupWindow = new PopupWindow((View) inflate.getRoot(), -1, -1, true);
            this.dialogBuyToken = popupWindow;
            popupWindow.setClippingEnabled(false);
            PopupWindow popupWindow2 = this.dialogBuyToken;
            UsefulKt.hideSystemUI(popupWindow2 != null ? popupWindow2.getContentView() : null);
            PopupWindow popupWindow3 = this.dialogBuyToken;
            if (popupWindow3 != null) {
                popupWindow3.setOnDismissListener(new PopupWindow.OnDismissListener() { // from class: com.blackhub.bronline.game.gui.familysystem.LogicForDialogBuyToken$$ExternalSyntheticLambda0
                    public /* synthetic */ LogicForDialogBuyToken$$ExternalSyntheticLambda0() {
                    }

                    @Override // android.widget.PopupWindow.OnDismissListener
                    public final void onDismiss() {
                        LogicForDialogBuyToken.lambda$4$lambda$0(LogicForDialogBuyToken.this);
                    }
                });
            }
            inflate.buttonCancel.setOnClickListener(new View.OnClickListener() { // from class: com.blackhub.bronline.game.gui.familysystem.LogicForDialogBuyToken$$ExternalSyntheticLambda1
                public /* synthetic */ LogicForDialogBuyToken$$ExternalSyntheticLambda1() {
                }

                @Override // android.view.View.OnClickListener
                public final void onClick(View view) {
                    LogicForDialogBuyToken.lambda$4$lambda$1(LogicForDialogBuyToken.this, view);
                }
            });
            inflate.editTextTokenValue.setGUIManager(mainRoot.getGuiManager());
            inflate.buttonBuyToken.setOnClickListener(new View.OnClickListener() { // from class: com.blackhub.bronline.game.gui.familysystem.LogicForDialogBuyToken$$ExternalSyntheticLambda2
                public final /* synthetic */ LogicForDialogBuyToken f$1;

                public /* synthetic */ LogicForDialogBuyToken$$ExternalSyntheticLambda2(LogicForDialogBuyToken this) {
                    r2 = this;
                }

                @Override // android.view.View.OnClickListener
                public final void onClick(View view) {
                    LogicForDialogBuyToken.lambda$4$lambda$3(FamiliesBuyTokenDialogBinding.this, r2, view);
                }
            });
        }
    }

    public static final void lambda$4$lambda$0(LogicForDialogBuyToken this$0) {
        Intrinsics.checkNotNullParameter(this$0, "this$0");
        this$0.mainRoot.closeAndroidsButtons();
    }

    public static final void lambda$4$lambda$1(LogicForDialogBuyToken this$0, View view) {
        Intrinsics.checkNotNullParameter(this$0, "this$0");
        PopupWindow popupWindow = this$0.dialogBuyToken;
        if (popupWindow != null) {
            popupWindow.dismiss();
        }
    }

    public static final void lambda$4$lambda$3(FamiliesBuyTokenDialogBinding _binding, LogicForDialogBuyToken this$0, View view) {
        ActionsWithJSON actionsWithJSON;
        Intrinsics.checkNotNullParameter(_binding, "$_binding");
        Intrinsics.checkNotNullParameter(this$0, "this$0");
        String valueOf = String.valueOf(_binding.editTextTokenValue.getText());
        int parseInt = ((StringsKt__StringsJVMKt.isBlank(valueOf) ^ true) || valueOf.length() > 0) ? Integer.parseInt(valueOf) : 0;
        if (parseInt != 0 && (actionsWithJSON = this$0.actionsWithJson) != null) {
            actionsWithJSON.buyToken(parseInt);
        }
        PopupWindow popupWindow = this$0.dialogBuyToken;
        if (popupWindow != null) {
            popupWindow.dismiss();
        }
    }

    public final void showDialog() {
        CustomEditText customEditText;
        Editable text;
        JNIActivity jNIActivity = this.mainActivity;
        if (jNIActivity != null) {
            PopupWindow popupWindow = this.dialogBuyToken;
            if (popupWindow != null) {
                popupWindow.showAtLocation(jNIActivity.getParentLayout(), 17, 0, 0);
            }
            FamiliesBuyTokenDialogBinding familiesBuyTokenDialogBinding = this.binding;
            if (familiesBuyTokenDialogBinding != null && (customEditText = familiesBuyTokenDialogBinding.editTextTokenValue) != null && (text = customEditText.getText()) != null) {
                text.clear();
            }
            FamiliesBuyTokenDialogBinding familiesBuyTokenDialogBinding2 = this.binding;
            TextView textView = familiesBuyTokenDialogBinding2 != null ? familiesBuyTokenDialogBinding2.textValueBuyToken : null;
            if (textView == null) {
                return;
            }
            textView.setText(jNIActivity.getString(R.string.common_string_with_bc, "0"));
        }
    }

    public final void updateCostInBC(int newCost) {
        JNIActivity jNIActivity = this.mainActivity;
        if (jNIActivity != null) {
            FamiliesBuyTokenDialogBinding familiesBuyTokenDialogBinding = this.binding;
            TextView textView = familiesBuyTokenDialogBinding != null ? familiesBuyTokenDialogBinding.textValueBuyToken : null;
            if (textView == null) {
                return;
            }
            textView.setText(jNIActivity.getString(R.string.common_string_with_bc, UsefulKt.getPriceWithDot(Integer.valueOf(newCost))));
        }
    }

    public final void closeDialog() {
        PopupWindow popupWindow = this.dialogBuyToken;
        if (popupWindow != null) {
            popupWindow.dismiss();
        }
    }

    public final void setNullableParameters() {
        closeDialog();
        this.binding = null;
        this.anim = null;
        this.dialogBuyToken = null;
    }
}
