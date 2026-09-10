package com.blackhub.bronline.game.gui.familysystem;

import android.view.LayoutInflater;
import android.view.View;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.widget.PopupWindow;
import android.widget.TextView;
import androidx.appcompat.widget.AppCompatButton;
import androidx.compose.runtime.internal.StabilityInferred;
import com.blackhub.bronline.R;
import com.blackhub.bronline.databinding.FamiliesApplyWindowBinding;
import com.blackhub.bronline.game.core.JNIActivity;
import com.blackhub.bronline.game.core.extension.AnyExtensionKt;
import com.blackhub.bronline.game.gui.UsefulKt;
import com.blackhub.bronline.game.gui.familysystem.data.FamilyNotificationData;
import com.blackhub.bronline.game.gui.familysystem.data.FamilySystemShopObj;
import com.blackhub.bronline.game.gui.familysystem.network.ActionsWithJSON;
import kotlin.Metadata;
import kotlin.Unit;
import kotlin.jvm.functions.Function0;
import kotlin.jvm.internal.Intrinsics;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

/* compiled from: LogicForDialogApply.kt */
@StabilityInferred(parameters = 0)
@Metadata(d1 = {"\u0000R\n\u0002\u0018\u0002\n\u0002\u0010\u0000\n\u0000\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0002\b\u0002\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0000\n\u0002\u0010\u0002\n\u0002\b\u0003\n\u0002\u0018\u0002\n\u0000\n\u0002\u0010\b\n\u0000\n\u0002\u0010\u000b\n\u0000\n\u0002\u0018\u0002\n\u0002\b\u0003\b\u0007\u0018\u00002\u00020\u0001B!\u0012\u0006\u0010\u0002\u001a\u00020\u0003\u0012\b\u0010\u0004\u001a\u0004\u0018\u00010\u0005\u0012\b\u0010\u0006\u001a\u0004\u0018\u00010\u0007¢\u0006\u0002\u0010\bJ\u0006\u0010\u000f\u001a\u00020\u0010J\u0006\u0010\u0011\u001a\u00020\u0010J5\u0010\u0012\u001a\u00020\u00102\b\u0010\u0013\u001a\u0004\u0018\u00010\u00142\b\u0010\u0015\u001a\u0004\u0018\u00010\u00162\b\u0010\u0017\u001a\u0004\u0018\u00010\u00182\b\u0010\u0019\u001a\u0004\u0018\u00010\u001aH\u0002¢\u0006\u0002\u0010\u001bJ3\u0010\u001c\u001a\u00020\u00102\b\u0010\u0013\u001a\u0004\u0018\u00010\u00142\b\u0010\u0015\u001a\u0004\u0018\u00010\u00162\b\u0010\u0017\u001a\u0004\u0018\u00010\u00182\b\u0010\u0019\u001a\u0004\u0018\u00010\u001a¢\u0006\u0002\u0010\u001bR\u0010\u0010\u0006\u001a\u0004\u0018\u00010\u0007X\u0082\u0004¢\u0006\u0002\n\u0000R\u0010\u0010\t\u001a\u0004\u0018\u00010\nX\u0082\u000e¢\u0006\u0002\n\u0000R\u0010\u0010\u000b\u001a\u0004\u0018\u00010\fX\u0082\u000e¢\u0006\u0002\n\u0000R\u0010\u0010\r\u001a\u0004\u0018\u00010\u000eX\u0082\u000e¢\u0006\u0002\n\u0000R\u0010\u0010\u0004\u001a\u0004\u0018\u00010\u0005X\u0082\u0004¢\u0006\u0002\n\u0000R\u000e\u0010\u0002\u001a\u00020\u0003X\u0082\u0004¢\u0006\u0002\n\u0000¨\u0006\u001d"}, d2 = {"Lcom/blackhub/bronline/game/gui/familysystem/LogicForDialogApply;", "", "mainRoot", "Lcom/blackhub/bronline/game/gui/familysystem/GUIFamilySystem;", "mainActivity", "Lcom/blackhub/bronline/game/core/JNIActivity;", "actionsWithJson", "Lcom/blackhub/bronline/game/gui/familysystem/network/ActionsWithJSON;", "(Lcom/blackhub/bronline/game/gui/familysystem/GUIFamilySystem;Lcom/blackhub/bronline/game/core/JNIActivity;Lcom/blackhub/bronline/game/gui/familysystem/network/ActionsWithJSON;)V", "anim", "Landroid/view/animation/Animation;", "bindingDialogApply", "Lcom/blackhub/bronline/databinding/FamiliesApplyWindowBinding;", "dialogApply", "Landroid/widget/PopupWindow;", "closeDialogApply", "", "setNullableParameters", "setTextInView", "thisItem", "Lcom/blackhub/bronline/game/gui/familysystem/data/FamilySystemShopObj;", "thisPrice", "", "ifClickLeave", "", "thisNotification", "Lcom/blackhub/bronline/game/gui/familysystem/data/FamilyNotificationData;", "(Lcom/blackhub/bronline/game/gui/familysystem/data/FamilySystemShopObj;Ljava/lang/Integer;Ljava/lang/Boolean;Lcom/blackhub/bronline/game/gui/familysystem/data/FamilyNotificationData;)V", "showDialog", "app_siteRelease"}, k = 1, mv = {1, 9, 0}, xi = 48)
/* loaded from: classes3.dex */
public final class LogicForDialogApply {
    public static final int $stable = 8;

    @Nullable
    public final ActionsWithJSON actionsWithJson;

    @Nullable
    public Animation anim;

    @Nullable
    public FamiliesApplyWindowBinding bindingDialogApply;

    @Nullable
    public PopupWindow dialogApply;

    @Nullable
    public final JNIActivity mainActivity;

    @NotNull
    public final GUIFamilySystem mainRoot;

    public LogicForDialogApply(@NotNull GUIFamilySystem mainRoot, @Nullable JNIActivity jNIActivity, @Nullable ActionsWithJSON actionsWithJSON) {
        Intrinsics.checkNotNullParameter(mainRoot, "mainRoot");
        this.mainRoot = mainRoot;
        this.mainActivity = jNIActivity;
        this.actionsWithJson = actionsWithJSON;
        Object systemService = jNIActivity != null ? jNIActivity.getSystemService("layout_inflater") : null;
        Intrinsics.checkNotNull(systemService, "null cannot be cast to non-null type android.view.LayoutInflater");
        FamiliesApplyWindowBinding inflate = FamiliesApplyWindowBinding.inflate((LayoutInflater) systemService);
        this.bindingDialogApply = inflate;
        if (inflate != null) {
            this.anim = AnimationUtils.loadAnimation(inflate.getRoot().getContext(), R.anim.button_click);
            PopupWindow popupWindow = new PopupWindow((View) inflate.getRoot(), -1, -1, true);
            this.dialogApply = popupWindow;
            popupWindow.setClippingEnabled(false);
            PopupWindow popupWindow2 = this.dialogApply;
            UsefulKt.hideSystemUI(popupWindow2 != null ? popupWindow2.getContentView() : null);
            PopupWindow popupWindow3 = this.dialogApply;
            if (popupWindow3 != null) {
                popupWindow3.setOnDismissListener(new PopupWindow.OnDismissListener() { // from class: com.blackhub.bronline.game.gui.familysystem.LogicForDialogApply$$ExternalSyntheticLambda2
                    public /* synthetic */ LogicForDialogApply$$ExternalSyntheticLambda2() {
                    }

                    @Override // android.widget.PopupWindow.OnDismissListener
                    public final void onDismiss() {
                        LogicForDialogApply.lambda$1$lambda$0(LogicForDialogApply.this);
                    }
                });
            }
        }
    }

    public static final void lambda$1$lambda$0(LogicForDialogApply this$0) {
        Intrinsics.checkNotNullParameter(this$0, "this$0");
        this$0.mainRoot.closeAndroidsButtons();
    }

    public final void showDialog(@Nullable FamilySystemShopObj thisItem, @Nullable Integer thisPrice, @Nullable Boolean ifClickLeave, @Nullable FamilyNotificationData thisNotification) {
        AppCompatButton appCompatButton;
        AppCompatButton appCompatButton2;
        PopupWindow popupWindow;
        setTextInView(thisItem, thisPrice, ifClickLeave, thisNotification);
        JNIActivity jNIActivity = this.mainActivity;
        if (jNIActivity != null && (popupWindow = this.dialogApply) != null) {
            popupWindow.showAtLocation(jNIActivity.getParentLayout(), 17, 0, 0);
        }
        FamiliesApplyWindowBinding familiesApplyWindowBinding = this.bindingDialogApply;
        if (familiesApplyWindowBinding != null && (appCompatButton2 = familiesApplyWindowBinding.buttonApply) != null) {
            appCompatButton2.setOnClickListener(new View.OnClickListener() { // from class: com.blackhub.bronline.game.gui.familysystem.LogicForDialogApply$$ExternalSyntheticLambda0
                public final /* synthetic */ FamilySystemShopObj f$1;
                public final /* synthetic */ Integer f$2;
                public final /* synthetic */ Boolean f$3;
                public final /* synthetic */ FamilyNotificationData f$4;

                public /* synthetic */ LogicForDialogApply$$ExternalSyntheticLambda0(FamilySystemShopObj thisItem2, Integer thisPrice2, Boolean ifClickLeave2, FamilyNotificationData thisNotification2) {
                    r2 = thisItem2;
                    r3 = thisPrice2;
                    r4 = ifClickLeave2;
                    r5 = thisNotification2;
                }

                @Override // android.view.View.OnClickListener
                public final void onClick(View view) {
                    LogicForDialogApply.showDialog$lambda$3(LogicForDialogApply.this, r2, r3, r4, r5, view);
                }
            });
        }
        FamiliesApplyWindowBinding familiesApplyWindowBinding2 = this.bindingDialogApply;
        if (familiesApplyWindowBinding2 == null || (appCompatButton = familiesApplyWindowBinding2.buttonCancel) == null) {
            return;
        }
        appCompatButton.setOnClickListener(new View.OnClickListener() { // from class: com.blackhub.bronline.game.gui.familysystem.LogicForDialogApply$$ExternalSyntheticLambda1
            public /* synthetic */ LogicForDialogApply$$ExternalSyntheticLambda1() {
            }

            @Override // android.view.View.OnClickListener
            public final void onClick(View view) {
                LogicForDialogApply.showDialog$lambda$4(LogicForDialogApply.this, view);
            }
        });
    }

    public static final void showDialog$lambda$3(LogicForDialogApply this$0, FamilySystemShopObj familySystemShopObj, Integer num, Boolean bool, FamilyNotificationData familyNotificationData, View view) {
        Intrinsics.checkNotNullParameter(this$0, "this$0");
        Animation animation = this$0.anim;
        if (animation != null) {
            AnyExtensionKt.setActionOnAnimationEnd(animation, new Function0<Unit>() { // from class: com.blackhub.bronline.game.gui.familysystem.LogicForDialogApply$showDialog$2$1
                public final /* synthetic */ Boolean $ifClickLeave;
                public final /* synthetic */ FamilyNotificationData $thisNotification;
                public final /* synthetic */ Integer $thisPrice;
                public final /* synthetic */ LogicForDialogApply this$0;

                /* JADX WARN: 'super' call moved to the top of the method (can break code semantics) */
                public LogicForDialogApply$showDialog$2$1(LogicForDialogApply this$02, Integer num2, Boolean bool2, FamilyNotificationData familyNotificationData2) {
                    super(0);
                    r2 = this$02;
                    r3 = num2;
                    r4 = bool2;
                    r5 = familyNotificationData2;
                }

                @Override // kotlin.jvm.functions.Function0
                public /* bridge */ /* synthetic */ Unit invoke() {
                    invoke2();
                    return Unit.INSTANCE;
                }

                /* renamed from: invoke */
                public final void invoke2() {
                    ActionsWithJSON actionsWithJSON;
                    ActionsWithJSON actionsWithJSON2;
                    GUIFamilySystem gUIFamilySystem;
                    ActionsWithJSON actionsWithJSON3;
                    if (FamilySystemShopObj.this != null) {
                        actionsWithJSON3 = r2.actionsWithJson;
                        if (actionsWithJSON3 != null) {
                            actionsWithJSON3.buyItemInShop(FamilySystemShopObj.this.getId());
                            return;
                        }
                        return;
                    }
                    if (r3 != null) {
                        gUIFamilySystem = r2.mainRoot;
                        gUIFamilySystem.upgradeFamily();
                        return;
                    }
                    if (r4 != null) {
                        actionsWithJSON2 = r2.actionsWithJson;
                        if (actionsWithJSON2 != null) {
                            actionsWithJSON2.sendPressButton(10);
                            return;
                        }
                        return;
                    }
                    if (r5 != null) {
                        actionsWithJSON = r2.actionsWithJson;
                        if (actionsWithJSON != null) {
                            actionsWithJSON.sendDeleteNotification(r5.getNotificationsId());
                        }
                        r2.closeDialogApply();
                    }
                }
            });
        }
        view.startAnimation(this$02.anim);
    }

    public static final void showDialog$lambda$4(LogicForDialogApply this$0, View view) {
        Intrinsics.checkNotNullParameter(this$0, "this$0");
        PopupWindow popupWindow = this$0.dialogApply;
        if (popupWindow != null) {
            popupWindow.dismiss();
        }
    }

    public final void setTextInView(FamilySystemShopObj thisItem, Integer thisPrice, Boolean ifClickLeave, FamilyNotificationData thisNotification) {
        TextView textView;
        String str;
        String string;
        JNIActivity jNIActivity = this.mainActivity;
        if (jNIActivity != null) {
            if (thisItem != null) {
                if (thisItem.getType() == 0) {
                    str = "семейных монет";
                } else {
                    str = "семейных жетонов";
                }
                FamiliesApplyWindowBinding familiesApplyWindowBinding = this.bindingDialogApply;
                if (familiesApplyWindowBinding != null) {
                    TextView textView2 = familiesApplyWindowBinding.dialogApplyBodyItem;
                    int typeId = thisItem.getTypeId();
                    if (typeId == 3) {
                        string = jNIActivity.getString(R.string.families_open_box, UsefulKt.getPriceWithSpaces(Integer.valueOf(thisItem.getPrice())), str);
                    } else if (typeId == 4) {
                        string = jNIActivity.getString(R.string.families_apply_buy_syndicate, UsefulKt.getPriceWithSpaces(Integer.valueOf(thisItem.getPrice())));
                    } else {
                        string = jNIActivity.getString(R.string.families_buy_item_in_shop, UsefulKt.getPriceWithSpaces(Integer.valueOf(thisItem.getPrice())), str);
                    }
                    textView2.setText(string);
                    return;
                }
                return;
            }
            if (thisPrice != null) {
                FamiliesApplyWindowBinding familiesApplyWindowBinding2 = this.bindingDialogApply;
                textView = familiesApplyWindowBinding2 != null ? familiesApplyWindowBinding2.dialogApplyBodyItem : null;
                if (textView == null) {
                    return;
                }
                textView.setText(jNIActivity.getString(R.string.families_apply_update, UsefulKt.getPriceWithSpaces(thisPrice)));
                return;
            }
            if (ifClickLeave != null) {
                FamiliesApplyWindowBinding familiesApplyWindowBinding3 = this.bindingDialogApply;
                textView = familiesApplyWindowBinding3 != null ? familiesApplyWindowBinding3.dialogApplyBodyItem : null;
                if (textView == null) {
                    return;
                }
                textView.setText(jNIActivity.getText(R.string.families_apply_leave));
                return;
            }
            if (thisNotification != null) {
                FamiliesApplyWindowBinding familiesApplyWindowBinding4 = this.bindingDialogApply;
                textView = familiesApplyWindowBinding4 != null ? familiesApplyWindowBinding4.dialogApplyBodyItem : null;
                if (textView == null) {
                    return;
                }
                textView.setText(jNIActivity.getText(R.string.families_apply_delete_notification));
            }
        }
    }

    public final void closeDialogApply() {
        PopupWindow popupWindow = this.dialogApply;
        if (popupWindow != null) {
            popupWindow.dismiss();
        }
    }

    public final void setNullableParameters() {
        closeDialogApply();
        this.dialogApply = null;
        this.bindingDialogApply = null;
        this.anim = null;
    }
}
