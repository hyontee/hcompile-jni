package com.blackhub.bronline.game.gui.familysystem;

import android.annotation.SuppressLint;
import android.text.Editable;
import android.view.View;
import android.view.animation.Animation;
import android.view.inputmethod.InputMethodManager;
import androidx.compose.runtime.internal.StabilityInferred;
import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.media3.extractor.text.ttml.TtmlNode;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;
import com.blackhub.bronline.databinding.FamiliesNotificationLayoutBinding;
import com.blackhub.bronline.game.common.UILayout;
import com.blackhub.bronline.game.core.JNIActivity;
import com.blackhub.bronline.game.gui.UsefulKt;
import com.blackhub.bronline.game.gui.familysystem.adapters.FamilyNotificationAdapter;
import com.blackhub.bronline.game.gui.familysystem.data.FamilyNotificationData;
import com.blackhub.bronline.game.gui.familysystem.network.ActionsWithJSON;
import java.util.ArrayList;
import java.util.List;
import kotlin.Metadata;
import kotlin.Unit;
import kotlin.jvm.functions.Function1;
import kotlin.jvm.internal.Intrinsics;
import kotlin.text.StringsKt__StringsJVMKt;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

/* compiled from: UIFamilyNotification.kt */
@StabilityInferred(parameters = 0)
@Metadata(d1 = {"\u0000v\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0002\b\u0002\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0002\b\u0002\n\u0002\u0010\u0002\n\u0002\u0018\u0002\n\u0000\n\u0002\u0010!\n\u0000\n\u0002\u0018\u0002\n\u0000\n\u0002\u0010\t\n\u0002\b\u0004\n\u0002\u0018\u0002\n\u0002\b\u0002\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0002\b\u0004\n\u0002\u0010\b\n\u0002\b\u0006\b\u0007\u0018\u00002\u00020\u0001B+\u0012\u0006\u0010\u0002\u001a\u00020\u0003\u0012\b\u0010\u0004\u001a\u0004\u0018\u00010\u0005\u0012\b\u0010\u0006\u001a\u0004\u0018\u00010\u0007\u0012\b\u0010\b\u001a\u0004\u0018\u00010\t¢\u0006\u0002\u0010\nJ\u000e\u0010\u001b\u001a\u00020\u00132\u0006\u0010\u001c\u001a\u00020\u000fJ\u000e\u0010\u001d\u001a\u00020\u00132\u0006\u0010\u001c\u001a\u00020\u000fJ\b\u0010\u001e\u001a\u00020\u001fH\u0016J\u001a\u0010 \u001a\u00020\u001f2\u0006\u0010!\u001a\u00020\"2\b\u0010#\u001a\u0004\u0018\u00010$H\u0016J\b\u0010%\u001a\u00020\u0013H\u0016J\b\u0010&\u001a\u00020\u0013H\u0016J\u000e\u0010'\u001a\u00020\u00132\u0006\u0010(\u001a\u00020)J\b\u0010*\u001a\u00020\u0013H\u0002J\b\u0010+\u001a\u00020\u0013H\u0002J\u0006\u0010,\u001a\u00020\u0013J\u0014\u0010-\u001a\u00020\u00132\f\u0010.\u001a\b\u0012\u0004\u0012\u00020\u000f0\u0016R\u0010\u0010\u0006\u001a\u0004\u0018\u00010\u0007X\u0082\u0004¢\u0006\u0002\n\u0000R\u000e\u0010\u000b\u001a\u00020\fX\u0082.¢\u0006\u0002\n\u0000R1\u0010\r\u001a%\u0012\u0013\u0012\u00110\u000f¢\u0006\f\b\u0010\u0012\b\b\u0011\u0012\u0004\b\b(\u0012\u0012\u0004\u0012\u00020\u0013\u0018\u00010\u000ej\u0004\u0018\u0001`\u0014X\u0082\u000e¢\u0006\u0002\n\u0000R\u0014\u0010\u0015\u001a\b\u0012\u0004\u0012\u00020\u000f0\u0016X\u0082\u0004¢\u0006\u0002\n\u0000R\u0010\u0010\b\u001a\u0004\u0018\u00010\tX\u0082\u0004¢\u0006\u0002\n\u0000R\u0010\u0010\u0004\u001a\u0004\u0018\u00010\u0005X\u0082\u0004¢\u0006\u0002\n\u0000R\u000e\u0010\u0002\u001a\u00020\u0003X\u0082\u0004¢\u0006\u0002\n\u0000R\u0010\u0010\u0017\u001a\u0004\u0018\u00010\u0018X\u0082\u000e¢\u0006\u0002\n\u0000R\u000e\u0010\u0019\u001a\u00020\u001aX\u0082\u000e¢\u0006\u0002\n\u0000¨\u0006/"}, d2 = {"Lcom/blackhub/bronline/game/gui/familysystem/UIFamilyNotification;", "Lcom/blackhub/bronline/game/common/UILayout;", "mainRoot", "Lcom/blackhub/bronline/game/gui/familysystem/GUIFamilySystem;", "mainActivity", "Lcom/blackhub/bronline/game/core/JNIActivity;", "actionsWithJSON", "Lcom/blackhub/bronline/game/gui/familysystem/network/ActionsWithJSON;", "logicForDialogApply", "Lcom/blackhub/bronline/game/gui/familysystem/LogicForDialogApply;", "(Lcom/blackhub/bronline/game/gui/familysystem/GUIFamilySystem;Lcom/blackhub/bronline/game/core/JNIActivity;Lcom/blackhub/bronline/game/gui/familysystem/network/ActionsWithJSON;Lcom/blackhub/bronline/game/gui/familysystem/LogicForDialogApply;)V", "binding", "Lcom/blackhub/bronline/databinding/FamiliesNotificationLayoutBinding;", "clickNotification", "Lkotlin/Function1;", "Lcom/blackhub/bronline/game/gui/familysystem/data/FamilyNotificationData;", "Lkotlin/ParameterName;", "name", "thisNotification", "", "Lcom/blackhub/bronline/game/gui/familysystem/OnClickNotification;", "familyNotifications", "", "notificationAdapter", "Lcom/blackhub/bronline/game/gui/familysystem/adapters/FamilyNotificationAdapter;", "oldTimer", "", "addNewNotification", "newNotification", "addOldNotification", "getView", "Landroid/view/View;", "onCreateView", "inflater", "Landroid/view/LayoutInflater;", TtmlNode.RUBY_CONTAINER, "Landroid/view/ViewGroup;", "onLayoutClose", "onLayoutShown", "removeNotification", "oldId", "", "setDataInView", "setLogicForClickNotification", "setNullableParameters", "setStartData", "notifications", "app_siteRelease"}, k = 1, mv = {1, 9, 0}, xi = 48)
@SuppressLint({"NotifyDataSetChanged"})
/* loaded from: classes3.dex */
public final class UIFamilyNotification extends UILayout {
    public static final int $stable = 8;

    @Nullable
    public final ActionsWithJSON actionsWithJSON;
    public FamiliesNotificationLayoutBinding binding;

    @Nullable
    public Function1<? super FamilyNotificationData, Unit> clickNotification;

    @NotNull
    public final List<FamilyNotificationData> familyNotifications;

    @Nullable
    public final LogicForDialogApply logicForDialogApply;

    @Nullable
    public final JNIActivity mainActivity;

    @NotNull
    public final GUIFamilySystem mainRoot;

    @Nullable
    public FamilyNotificationAdapter notificationAdapter;
    public long oldTimer;

    @Override // com.blackhub.bronline.game.common.UILayout
    public void onLayoutClose() {
    }

    @Override // com.blackhub.bronline.game.common.UILayout
    public void onLayoutShown() {
    }

    public UIFamilyNotification(@NotNull GUIFamilySystem mainRoot, @Nullable JNIActivity jNIActivity, @Nullable ActionsWithJSON actionsWithJSON, @Nullable LogicForDialogApply logicForDialogApply) {
        Intrinsics.checkNotNullParameter(mainRoot, "mainRoot");
        this.mainRoot = mainRoot;
        this.mainActivity = jNIActivity;
        this.actionsWithJSON = actionsWithJSON;
        this.logicForDialogApply = logicForDialogApply;
        this.familyNotifications = new ArrayList();
    }

    public final void setStartData(@NotNull List<FamilyNotificationData> notifications) {
        Intrinsics.checkNotNullParameter(notifications, "notifications");
        List<FamilyNotificationData> list = this.familyNotifications;
        list.clear();
        list.addAll(notifications);
        FamilyNotificationAdapter familyNotificationAdapter = this.notificationAdapter;
        if (familyNotificationAdapter != null) {
            familyNotificationAdapter.notifyDataSetChanged();
        }
        FamiliesNotificationLayoutBinding familiesNotificationLayoutBinding = this.binding;
        if (familiesNotificationLayoutBinding == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            familiesNotificationLayoutBinding = null;
        }
        if (this.mainRoot.getIfLeader() == 1) {
            familiesNotificationLayoutBinding.addNewNotification.setVisibility(0);
            familiesNotificationLayoutBinding.editTextNotification.setVisibility(0);
        } else {
            familiesNotificationLayoutBinding.addNewNotification.setVisibility(4);
            familiesNotificationLayoutBinding.editTextNotification.setVisibility(4);
        }
    }

    public final void addNewNotification(@NotNull FamilyNotificationData newNotification) {
        Intrinsics.checkNotNullParameter(newNotification, "newNotification");
        this.familyNotifications.add(0, newNotification);
        if (this.familyNotifications.size() > 5) {
            this.familyNotifications.remove(5);
        }
        FamilyNotificationAdapter familyNotificationAdapter = this.notificationAdapter;
        if (familyNotificationAdapter != null) {
            familyNotificationAdapter.notifyDataSetChanged();
        }
    }

    public final void addOldNotification(@NotNull FamilyNotificationData newNotification) {
        Intrinsics.checkNotNullParameter(newNotification, "newNotification");
        this.familyNotifications.add(newNotification);
        FamilyNotificationAdapter familyNotificationAdapter = this.notificationAdapter;
        if (familyNotificationAdapter != null) {
            familyNotificationAdapter.notifyDataSetChanged();
        }
    }

    public final void removeNotification(int oldId) {
        if (this.familyNotifications.size() > 0) {
            int size = this.familyNotifications.size();
            for (int i = 0; i < size; i++) {
                if (this.familyNotifications.get(i).getNotificationsId() == oldId) {
                    this.familyNotifications.remove(i);
                    FamilyNotificationAdapter familyNotificationAdapter = this.notificationAdapter;
                    if (familyNotificationAdapter != null) {
                        familyNotificationAdapter.notifyItemRemoved(i);
                        return;
                    }
                    return;
                }
            }
        }
    }

    @Override // com.blackhub.bronline.game.common.UILayout
    @NotNull
    /* renamed from: getView */
    public View getMViewRoot() {
        FamiliesNotificationLayoutBinding familiesNotificationLayoutBinding = this.binding;
        if (familiesNotificationLayoutBinding == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            familiesNotificationLayoutBinding = null;
        }
        ConstraintLayout root = familiesNotificationLayoutBinding.getRoot();
        Intrinsics.checkNotNullExpressionValue(root, "getRoot(...)");
        return root;
    }

    /*  JADX ERROR: JadxRuntimeException in pass: ProcessVariables
        jadx.core.utils.exceptions.JadxRuntimeException: Method arg registers not loaded: com.blackhub.bronline.game.gui.familysystem.UIFamilyNotification$$ExternalSyntheticLambda0.<init>(com.blackhub.bronline.game.gui.familysystem.UIFamilyNotification, com.blackhub.bronline.databinding.FamiliesNotificationLayoutBinding, android.view.animation.Animation):void, class status: GENERATED_AND_UNLOADED
        	at jadx.core.dex.nodes.MethodNode.getArgRegs(MethodNode.java:290)
        	at jadx.core.dex.visitors.regions.variables.ProcessVariables$1.isArgUnused(ProcessVariables.java:146)
        	at jadx.core.dex.visitors.regions.variables.ProcessVariables$1.lambda$isVarUnused$0(ProcessVariables.java:131)
        	at jadx.core.utils.ListUtils.allMatch(ListUtils.java:193)
        	at jadx.core.dex.visitors.regions.variables.ProcessVariables$1.isVarUnused(ProcessVariables.java:131)
        	at jadx.core.dex.visitors.regions.variables.ProcessVariables$1.processBlock(ProcessVariables.java:82)
        	at jadx.core.dex.visitors.regions.DepthRegionTraversal.traverseInternal(DepthRegionTraversal.java:64)
        	at jadx.core.dex.visitors.regions.DepthRegionTraversal.lambda$traverseInternal$0(DepthRegionTraversal.java:68)
        	at java.base/java.util.ArrayList.forEach(ArrayList.java:1511)
        	at jadx.core.dex.visitors.regions.DepthRegionTraversal.traverseInternal(DepthRegionTraversal.java:68)
        	at jadx.core.dex.visitors.regions.DepthRegionTraversal.traverse(DepthRegionTraversal.java:19)
        	at jadx.core.dex.visitors.regions.variables.ProcessVariables.removeUnusedResults(ProcessVariables.java:73)
        	at jadx.core.dex.visitors.regions.variables.ProcessVariables.visit(ProcessVariables.java:48)
        */
    @Override // com.blackhub.bronline.game.common.UILayout
    @org.jetbrains.annotations.NotNull
    public android.view.View onCreateView(@org.jetbrains.annotations.NotNull android.view.LayoutInflater r5, @org.jetbrains.annotations.Nullable android.view.ViewGroup r6) {
        /*
            r4 = this;
            java.lang.String r6 = "inflater"
            kotlin.jvm.internal.Intrinsics.checkNotNullParameter(r5, r6)
            com.blackhub.bronline.databinding.FamiliesNotificationLayoutBinding r5 = com.blackhub.bronline.databinding.FamiliesNotificationLayoutBinding.inflate(r5)
            java.lang.String r6 = "inflate(...)"
            kotlin.jvm.internal.Intrinsics.checkNotNullExpressionValue(r5, r6)
            r4.binding = r5
            r6 = 0
            java.lang.String r0 = "binding"
            if (r5 != 0) goto L19
            kotlin.jvm.internal.Intrinsics.throwUninitializedPropertyAccessException(r0)
            r5 = r6
        L19:
            androidx.constraintlayout.widget.ConstraintLayout r5 = r5.getRoot()
            android.content.Context r5 = r5.getContext()
            r1 = 2130771993(0x7f010019, float:1.7147092E38)
            android.view.animation.Animation r5 = android.view.animation.AnimationUtils.loadAnimation(r5, r1)
            r4.setLogicForClickNotification()
            r4.setDataInView()
            com.blackhub.bronline.databinding.FamiliesNotificationLayoutBinding r1 = r4.binding
            if (r1 != 0) goto L36
            kotlin.jvm.internal.Intrinsics.throwUninitializedPropertyAccessException(r0)
            r1 = r6
        L36:
            android.widget.ImageView r2 = r1.addNewNotification
            com.blackhub.bronline.game.gui.familysystem.UIFamilyNotification$$ExternalSyntheticLambda0 r3 = new com.blackhub.bronline.game.gui.familysystem.UIFamilyNotification$$ExternalSyntheticLambda0
            r3.<init>()
            r2.setOnClickListener(r3)
            com.blackhub.bronline.databinding.FamiliesNotificationLayoutBinding r5 = r4.binding
            if (r5 != 0) goto L48
            kotlin.jvm.internal.Intrinsics.throwUninitializedPropertyAccessException(r0)
            goto L49
        L48:
            r6 = r5
        L49:
            androidx.constraintlayout.widget.ConstraintLayout r5 = r6.getRoot()
            java.lang.String r6 = "getRoot(...)"
            kotlin.jvm.internal.Intrinsics.checkNotNullExpressionValue(r5, r6)
            return r5
        */
        throw new UnsupportedOperationException("Method not decompiled: com.blackhub.bronline.game.gui.familysystem.UIFamilyNotification.onCreateView(android.view.LayoutInflater, android.view.ViewGroup):android.view.View");
    }

    public static final void onCreateView$lambda$3$lambda$2(UIFamilyNotification this$0, FamiliesNotificationLayoutBinding this_apply, Animation animation, View view) {
        Intrinsics.checkNotNullParameter(this$0, "this$0");
        Intrinsics.checkNotNullParameter(this_apply, "$this_apply");
        FamiliesNotificationLayoutBinding familiesNotificationLayoutBinding = null;
        if (System.currentTimeMillis() - this$0.oldTimer > 300) {
            this$0.oldTimer = System.currentTimeMillis();
            Editable text = this_apply.editTextNotification.getText();
            Intrinsics.checkNotNullExpressionValue(text, "getText(...)");
            if (text.length() <= 0) {
                Intrinsics.checkNotNullExpressionValue(this_apply.editTextNotification.getText(), "getText(...)");
                if (!(!StringsKt__StringsJVMKt.isBlank(r0))) {
                    ActionsWithJSON actionsWithJSON = this$0.actionsWithJSON;
                    if (actionsWithJSON != null) {
                        actionsWithJSON.sendMessageError("Введите текст в поле для ввода");
                    }
                }
            }
            view.startAnimation(animation);
            ActionsWithJSON actionsWithJSON2 = this$0.actionsWithJSON;
            if (actionsWithJSON2 != null) {
                actionsWithJSON2.sendMessageForServer(this_apply.editTextNotification.getText().toString());
            }
            this_apply.editTextNotification.getText().clear();
            Object systemService = this$0.getContext().getSystemService("input_method");
            Intrinsics.checkNotNull(systemService, "null cannot be cast to non-null type android.view.inputmethod.InputMethodManager");
            InputMethodManager inputMethodManager = (InputMethodManager) systemService;
            FamiliesNotificationLayoutBinding familiesNotificationLayoutBinding2 = this$0.binding;
            if (familiesNotificationLayoutBinding2 == null) {
                Intrinsics.throwUninitializedPropertyAccessException("binding");
                familiesNotificationLayoutBinding2 = null;
            }
            inputMethodManager.hideSoftInputFromWindow(familiesNotificationLayoutBinding2.getRoot().getWindowToken(), 0);
            this_apply.editTextNotification.setFocusable(false);
            this_apply.editTextNotification.setFocusableInTouchMode(true);
        }
        FamiliesNotificationLayoutBinding familiesNotificationLayoutBinding3 = this$0.binding;
        if (familiesNotificationLayoutBinding3 == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
        } else {
            familiesNotificationLayoutBinding = familiesNotificationLayoutBinding3;
        }
        UsefulKt.hideSystemUI(familiesNotificationLayoutBinding.getRoot());
    }

    public final void setLogicForClickNotification() {
        this.clickNotification = new Function1<FamilyNotificationData, Unit>() { // from class: com.blackhub.bronline.game.gui.familysystem.UIFamilyNotification$setLogicForClickNotification$1
            public UIFamilyNotification$setLogicForClickNotification$1() {
            }

            @Override // kotlin.jvm.functions.Function1
            public /* bridge */ /* synthetic */ Unit invoke(FamilyNotificationData familyNotificationData) {
                invoke2(familyNotificationData);
                return Unit.INSTANCE;
            }

            /* JADX WARN: Code restructure failed: missing block: B:3:0x0013, code lost:
            
                r0 = com.blackhub.bronline.game.gui.familysystem.UIFamilyNotification.this.logicForDialogApply;
             */
            /* renamed from: invoke */
            /*
                Code decompiled incorrectly, please refer to instructions dump.
                To view partially-correct code enable 'Show inconsistent code' option in preferences
            */
            public void invoke2(@org.jetbrains.annotations.NotNull com.blackhub.bronline.game.gui.familysystem.data.FamilyNotificationData r3) {
                /*
                    r2 = this;
                    java.lang.String r0 = "thisNotification"
                    kotlin.jvm.internal.Intrinsics.checkNotNullParameter(r3, r0)
                    com.blackhub.bronline.game.gui.familysystem.UIFamilyNotification r0 = com.blackhub.bronline.game.gui.familysystem.UIFamilyNotification.this
                    com.blackhub.bronline.game.gui.familysystem.GUIFamilySystem r0 = com.blackhub.bronline.game.gui.familysystem.UIFamilyNotification.access$getMainRoot$p(r0)
                    int r0 = r0.getIfLeader()
                    r1 = 1
                    if (r0 != r1) goto L1f
                    com.blackhub.bronline.game.gui.familysystem.UIFamilyNotification r0 = com.blackhub.bronline.game.gui.familysystem.UIFamilyNotification.this
                    com.blackhub.bronline.game.gui.familysystem.LogicForDialogApply r0 = com.blackhub.bronline.game.gui.familysystem.UIFamilyNotification.access$getLogicForDialogApply$p(r0)
                    if (r0 == 0) goto L1f
                    r1 = 0
                    r0.showDialog(r1, r1, r1, r3)
                L1f:
                    return
                */
                throw new UnsupportedOperationException("Method not decompiled: com.blackhub.bronline.game.gui.familysystem.UIFamilyNotification$setLogicForClickNotification$1.invoke2(com.blackhub.bronline.game.gui.familysystem.data.FamilyNotificationData):void");
            }
        };
    }

    public final void setDataInView() {
        this.notificationAdapter = new FamilyNotificationAdapter(this.familyNotifications, this.mainActivity, this.clickNotification);
        FamiliesNotificationLayoutBinding familiesNotificationLayoutBinding = this.binding;
        FamiliesNotificationLayoutBinding familiesNotificationLayoutBinding2 = null;
        if (familiesNotificationLayoutBinding == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            familiesNotificationLayoutBinding = null;
        }
        RecyclerView recyclerView = familiesNotificationLayoutBinding.familyNotifications;
        FamiliesNotificationLayoutBinding familiesNotificationLayoutBinding3 = this.binding;
        if (familiesNotificationLayoutBinding3 == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
        } else {
            familiesNotificationLayoutBinding2 = familiesNotificationLayoutBinding3;
        }
        recyclerView.setLayoutManager(new LinearLayoutManager(familiesNotificationLayoutBinding2.getRoot().getContext(), 1, false));
        recyclerView.setAdapter(this.notificationAdapter);
    }

    public final void setNullableParameters() {
        this.notificationAdapter = null;
        this.clickNotification = null;
        this.familyNotifications.clear();
    }
}
