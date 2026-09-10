package com.blackhub.bronline.game.gui.familysystem;

import android.view.View;
import android.view.animation.Animation;
import androidx.compose.runtime.internal.StabilityInferred;
import androidx.core.widget.NestedScrollView;
import androidx.media3.extractor.text.ttml.TtmlNode;
import com.blackhub.bronline.databinding.FamiliesMainLayoutBinding;
import com.blackhub.bronline.game.common.TimeChecker;
import com.blackhub.bronline.game.common.UILayout;
import com.blackhub.bronline.game.core.extension.AnyExtensionKt;
import com.blackhub.bronline.game.gui.familysystem.network.ActionsWithJSON;
import kotlin.Metadata;
import kotlin.Unit;
import kotlin.jvm.functions.Function0;
import kotlin.jvm.internal.Intrinsics;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

/* compiled from: UIMailLayout.kt */
@StabilityInferred(parameters = 0)
@Metadata(d1 = {"\u0000@\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0002\b\u0002\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0002\b\u0002\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0000\n\u0002\u0010\u0002\n\u0002\b\u0002\b\u0007\u0018\u00002\u00020\u0001B\u0017\u0012\u0006\u0010\u0002\u001a\u00020\u0003\u0012\b\u0010\u0004\u001a\u0004\u0018\u00010\u0005¢\u0006\u0002\u0010\u0006J\b\u0010\u000b\u001a\u00020\fH\u0016J\u001a\u0010\r\u001a\u00020\f2\u0006\u0010\u000e\u001a\u00020\u000f2\b\u0010\u0010\u001a\u0004\u0018\u00010\u0011H\u0016J\b\u0010\u0012\u001a\u00020\u0013H\u0016J\b\u0010\u0014\u001a\u00020\u0013H\u0016R\u0010\u0010\u0004\u001a\u0004\u0018\u00010\u0005X\u0082\u0004¢\u0006\u0002\n\u0000R\u000e\u0010\u0007\u001a\u00020\bX\u0082.¢\u0006\u0002\n\u0000R\u000e\u0010\u0002\u001a\u00020\u0003X\u0082\u0004¢\u0006\u0002\n\u0000R\u000e\u0010\t\u001a\u00020\nX\u0082\u0004¢\u0006\u0002\n\u0000¨\u0006\u0015"}, d2 = {"Lcom/blackhub/bronline/game/gui/familysystem/UIMailLayout;", "Lcom/blackhub/bronline/game/common/UILayout;", "mainRoot", "Lcom/blackhub/bronline/game/gui/familysystem/GUIFamilySystem;", "actionsWithJSON", "Lcom/blackhub/bronline/game/gui/familysystem/network/ActionsWithJSON;", "(Lcom/blackhub/bronline/game/gui/familysystem/GUIFamilySystem;Lcom/blackhub/bronline/game/gui/familysystem/network/ActionsWithJSON;)V", "binding", "Lcom/blackhub/bronline/databinding/FamiliesMainLayoutBinding;", "timeChecker", "Lcom/blackhub/bronline/game/common/TimeChecker;", "getView", "Landroid/view/View;", "onCreateView", "inflater", "Landroid/view/LayoutInflater;", TtmlNode.RUBY_CONTAINER, "Landroid/view/ViewGroup;", "onLayoutClose", "", "onLayoutShown", "app_siteRelease"}, k = 1, mv = {1, 9, 0}, xi = 48)
/* loaded from: classes3.dex */
public final class UIMailLayout extends UILayout {
    public static final int $stable = 8;

    @Nullable
    public final ActionsWithJSON actionsWithJSON;
    public FamiliesMainLayoutBinding binding;

    @NotNull
    public final GUIFamilySystem mainRoot;

    @NotNull
    public final TimeChecker timeChecker;

    @Override // com.blackhub.bronline.game.common.UILayout
    public void onLayoutClose() {
    }

    @Override // com.blackhub.bronline.game.common.UILayout
    public void onLayoutShown() {
    }

    public UIMailLayout(@NotNull GUIFamilySystem mainRoot, @Nullable ActionsWithJSON actionsWithJSON) {
        Intrinsics.checkNotNullParameter(mainRoot, "mainRoot");
        this.mainRoot = mainRoot;
        this.actionsWithJSON = actionsWithJSON;
        this.timeChecker = new TimeChecker();
    }

    @Override // com.blackhub.bronline.game.common.UILayout
    @NotNull
    /* renamed from: getView */
    public View getMViewRoot() {
        FamiliesMainLayoutBinding familiesMainLayoutBinding = this.binding;
        if (familiesMainLayoutBinding == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            familiesMainLayoutBinding = null;
        }
        NestedScrollView root = familiesMainLayoutBinding.getRoot();
        Intrinsics.checkNotNullExpressionValue(root, "getRoot(...)");
        return root;
    }

    /*  JADX ERROR: JadxRuntimeException in pass: ProcessVariables
        jadx.core.utils.exceptions.JadxRuntimeException: Method arg registers not loaded: com.blackhub.bronline.game.gui.familysystem.UIMailLayout$$ExternalSyntheticLambda0.<init>(com.blackhub.bronline.game.gui.familysystem.UIMailLayout, android.view.animation.Animation):void, class status: GENERATED_AND_UNLOADED
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
            com.blackhub.bronline.databinding.FamiliesMainLayoutBinding r5 = com.blackhub.bronline.databinding.FamiliesMainLayoutBinding.inflate(r5)
            java.lang.String r6 = "inflate(...)"
            kotlin.jvm.internal.Intrinsics.checkNotNullExpressionValue(r5, r6)
            r4.binding = r5
            r6 = 0
            java.lang.String r0 = "binding"
            if (r5 != 0) goto L19
            kotlin.jvm.internal.Intrinsics.throwUninitializedPropertyAccessException(r0)
            r5 = r6
        L19:
            androidx.core.widget.NestedScrollView r5 = r5.getRoot()
            android.content.Context r5 = r5.getContext()
            r1 = 2130771993(0x7f010019, float:1.7147092E38)
            android.view.animation.Animation r5 = android.view.animation.AnimationUtils.loadAnimation(r5, r1)
            com.blackhub.bronline.databinding.FamiliesMainLayoutBinding r1 = r4.binding
            if (r1 != 0) goto L30
            kotlin.jvm.internal.Intrinsics.throwUninitializedPropertyAccessException(r0)
            r1 = r6
        L30:
            android.view.View r2 = r1.familyItem1
            com.blackhub.bronline.game.gui.familysystem.UIMailLayout$$ExternalSyntheticLambda0 r3 = new com.blackhub.bronline.game.gui.familysystem.UIMailLayout$$ExternalSyntheticLambda0
            r3.<init>()
            r2.setOnClickListener(r3)
            android.view.View r2 = r1.familyItem2
            com.blackhub.bronline.game.gui.familysystem.UIMailLayout$$ExternalSyntheticLambda1 r3 = new com.blackhub.bronline.game.gui.familysystem.UIMailLayout$$ExternalSyntheticLambda1
            r3.<init>()
            r2.setOnClickListener(r3)
            android.view.View r2 = r1.familyItem3
            com.blackhub.bronline.game.gui.familysystem.UIMailLayout$$ExternalSyntheticLambda2 r3 = new com.blackhub.bronline.game.gui.familysystem.UIMailLayout$$ExternalSyntheticLambda2
            r3.<init>()
            r2.setOnClickListener(r3)
            android.view.View r2 = r1.familyItem4
            com.blackhub.bronline.game.gui.familysystem.UIMailLayout$$ExternalSyntheticLambda3 r3 = new com.blackhub.bronline.game.gui.familysystem.UIMailLayout$$ExternalSyntheticLambda3
            r3.<init>()
            r2.setOnClickListener(r3)
            android.view.View r1 = r1.familyItem5
            com.blackhub.bronline.game.gui.familysystem.UIMailLayout$$ExternalSyntheticLambda4 r2 = new com.blackhub.bronline.game.gui.familysystem.UIMailLayout$$ExternalSyntheticLambda4
            r2.<init>()
            r1.setOnClickListener(r2)
            com.blackhub.bronline.databinding.FamiliesMainLayoutBinding r5 = r4.binding
            if (r5 != 0) goto L6a
            kotlin.jvm.internal.Intrinsics.throwUninitializedPropertyAccessException(r0)
            goto L6b
        L6a:
            r6 = r5
        L6b:
            androidx.core.widget.NestedScrollView r5 = r6.getRoot()
            java.lang.String r6 = "getRoot(...)"
            kotlin.jvm.internal.Intrinsics.checkNotNullExpressionValue(r5, r6)
            return r5
        */
        throw new UnsupportedOperationException("Method not decompiled: com.blackhub.bronline.game.gui.familysystem.UIMailLayout.onCreateView(android.view.LayoutInflater, android.view.ViewGroup):android.view.View");
    }

    public static final void onCreateView$lambda$5$lambda$0(UIMailLayout this$0, Animation animation, View view) {
        Intrinsics.checkNotNullParameter(this$0, "this$0");
        if (this$0.timeChecker.ifAccess(500L)) {
            Intrinsics.checkNotNull(animation);
            AnyExtensionKt.setActionOnAnimationEnd(animation, new Function0<Unit>() { // from class: com.blackhub.bronline.game.gui.familysystem.UIMailLayout$onCreateView$1$1$1
                public UIMailLayout$onCreateView$1$1$1() {
                    super(0);
                }

                @Override // kotlin.jvm.functions.Function0
                public /* bridge */ /* synthetic */ Unit invoke() {
                    invoke2();
                    return Unit.INSTANCE;
                }

                /* renamed from: invoke */
                public final void invoke2() {
                    GUIFamilySystem gUIFamilySystem;
                    gUIFamilySystem = UIMailLayout.this.mainRoot;
                    gUIFamilySystem.listenerLayout(4);
                }
            });
            view.startAnimation(animation);
        }
    }

    public static final void onCreateView$lambda$5$lambda$1(UIMailLayout this$0, Animation animation, View view) {
        Intrinsics.checkNotNullParameter(this$0, "this$0");
        if (this$0.timeChecker.ifAccess(500L)) {
            Intrinsics.checkNotNull(animation);
            AnyExtensionKt.setActionOnAnimationEnd(animation, new Function0<Unit>() { // from class: com.blackhub.bronline.game.gui.familysystem.UIMailLayout$onCreateView$1$2$1
                public UIMailLayout$onCreateView$1$2$1() {
                    super(0);
                }

                @Override // kotlin.jvm.functions.Function0
                public /* bridge */ /* synthetic */ Unit invoke() {
                    invoke2();
                    return Unit.INSTANCE;
                }

                /* renamed from: invoke */
                public final void invoke2() {
                    ActionsWithJSON actionsWithJSON;
                    GUIFamilySystem gUIFamilySystem;
                    actionsWithJSON = UIMailLayout.this.actionsWithJSON;
                    if (actionsWithJSON != null) {
                        actionsWithJSON.sendGetLayout(2);
                    }
                    gUIFamilySystem = UIMailLayout.this.mainRoot;
                    gUIFamilySystem.setIfFromMainMenuForCarPark(true);
                }
            });
            view.startAnimation(animation);
        }
    }

    public static final void onCreateView$lambda$5$lambda$2(UIMailLayout this$0, Animation animation, View view) {
        Intrinsics.checkNotNullParameter(this$0, "this$0");
        if (this$0.timeChecker.ifAccess(500L)) {
            Intrinsics.checkNotNull(animation);
            AnyExtensionKt.setActionOnAnimationEnd(animation, new Function0<Unit>() { // from class: com.blackhub.bronline.game.gui.familysystem.UIMailLayout$onCreateView$1$3$1
                public UIMailLayout$onCreateView$1$3$1() {
                    super(0);
                }

                @Override // kotlin.jvm.functions.Function0
                public /* bridge */ /* synthetic */ Unit invoke() {
                    invoke2();
                    return Unit.INSTANCE;
                }

                /* renamed from: invoke */
                public final void invoke2() {
                    GUIFamilySystem gUIFamilySystem;
                    gUIFamilySystem = UIMailLayout.this.mainRoot;
                    gUIFamilySystem.listenerLayout(3);
                }
            });
            view.startAnimation(animation);
        }
    }

    public static final void onCreateView$lambda$5$lambda$3(UIMailLayout this$0, Animation animation, View view) {
        Intrinsics.checkNotNullParameter(this$0, "this$0");
        if (this$0.timeChecker.ifAccess(500L)) {
            Intrinsics.checkNotNull(animation);
            AnyExtensionKt.setActionOnAnimationEnd(animation, new Function0<Unit>() { // from class: com.blackhub.bronline.game.gui.familysystem.UIMailLayout$onCreateView$1$4$1
                public UIMailLayout$onCreateView$1$4$1() {
                    super(0);
                }

                @Override // kotlin.jvm.functions.Function0
                public /* bridge */ /* synthetic */ Unit invoke() {
                    invoke2();
                    return Unit.INSTANCE;
                }

                /* renamed from: invoke */
                public final void invoke2() {
                    ActionsWithJSON actionsWithJSON;
                    GUIFamilySystem gUIFamilySystem;
                    actionsWithJSON = UIMailLayout.this.actionsWithJSON;
                    if (actionsWithJSON != null) {
                        actionsWithJSON.sendGetLayout(4);
                    }
                    gUIFamilySystem = UIMailLayout.this.mainRoot;
                    gUIFamilySystem.setIfFromMainMenuForUpgrade(true);
                }
            });
            view.startAnimation(animation);
        }
    }

    public static final void onCreateView$lambda$5$lambda$4(UIMailLayout this$0, Animation animation, View view) {
        Intrinsics.checkNotNullParameter(this$0, "this$0");
        if (this$0.timeChecker.ifAccess(500L)) {
            Intrinsics.checkNotNull(animation);
            AnyExtensionKt.setActionOnAnimationEnd(animation, new Function0<Unit>() { // from class: com.blackhub.bronline.game.gui.familysystem.UIMailLayout$onCreateView$1$5$1
                public UIMailLayout$onCreateView$1$5$1() {
                    super(0);
                }

                @Override // kotlin.jvm.functions.Function0
                public /* bridge */ /* synthetic */ Unit invoke() {
                    invoke2();
                    return Unit.INSTANCE;
                }

                /* renamed from: invoke */
                public final void invoke2() {
                    ActionsWithJSON actionsWithJSON;
                    actionsWithJSON = UIMailLayout.this.actionsWithJSON;
                    if (actionsWithJSON != null) {
                        actionsWithJSON.sendGetLayout(7);
                    }
                }
            });
            view.startAnimation(animation);
        }
    }
}
