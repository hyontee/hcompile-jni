package com.blackhub.bronline.game.gui.familysystem;

import android.annotation.SuppressLint;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import androidx.compose.runtime.internal.StabilityInferred;
import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.media3.extractor.text.ttml.TtmlNode;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;
import com.blackhub.bronline.R;
import com.blackhub.bronline.databinding.FamiliesMenuCarParkLayoutBinding;
import com.blackhub.bronline.game.common.UILayout;
import com.blackhub.bronline.game.core.JNIActivity;
import com.blackhub.bronline.game.gui.familysystem.adapters.FamilyMenuMainAdapter;
import com.blackhub.bronline.game.gui.familysystem.adapters.FamilyPlayersActionAdapter;
import com.blackhub.bronline.game.gui.familysystem.data.FamilyMainItemData;
import com.blackhub.bronline.game.gui.familysystem.data.FamilyPlayersAction;
import com.blackhub.bronline.game.gui.familysystem.network.ActionsWithJSON;
import java.util.ArrayList;
import java.util.List;
import kotlin.Metadata;
import kotlin.Unit;
import kotlin.jvm.functions.Function2;
import kotlin.jvm.functions.Function3;
import kotlin.jvm.internal.Intrinsics;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

/* compiled from: UIMenuCarPark.kt */
@StabilityInferred(parameters = 0)
@Metadata(d1 = {"\u0000\u009a\u0001\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0002\b\u0002\n\u0002\u0010!\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0002\b\u0002\n\u0002\u0010\b\n\u0000\n\u0002\u0010\u0002\n\u0002\u0018\u0002\n\u0002\b\u0005\n\u0002\u0010\t\n\u0002\b\u0002\n\u0002\u0018\u0002\n\u0002\b\u0002\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0002\b\b\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0002\b\b\n\u0002\u0010\u000e\n\u0002\b\f\n\u0002\u0010\u000b\n\u0002\b\u0004\b\u0007\u0018\u00002\u00020\u0001B\u0019\u0012\b\u0010\u0002\u001a\u0004\u0018\u00010\u0003\u0012\b\u0010\u0004\u001a\u0004\u0018\u00010\u0005¢\u0006\u0002\u0010\u0006J\u0010\u0010+\u001a\u00020\u001b2\u0006\u0010,\u001a\u00020\u0019H\u0002J\u0018\u0010-\u001a\u00020\u001b2\u0006\u0010.\u001a\u00020\u000f2\u0006\u0010/\u001a\u00020\u0019H\u0002J\b\u00100\u001a\u00020(H\u0016J\u001a\u00101\u001a\u00020(2\u0006\u00102\u001a\u0002032\b\u00104\u001a\u0004\u0018\u000105H\u0016J\b\u00106\u001a\u00020\u001bH\u0016J\b\u00107\u001a\u00020\u001bH\u0016J\u0010\u00108\u001a\u00020\u001b2\u0006\u00109\u001a\u00020\u0019H\u0003J\b\u0010:\u001a\u00020\u001bH\u0003J$\u0010;\u001a\u00020\u001b2\f\u0010<\u001a\b\u0012\u0004\u0012\u00020\u00190\b2\f\u0010=\u001a\b\u0012\u0004\u0012\u00020>0\bH\u0007J\u0016\u0010?\u001a\u00020\u001b2\u0006\u0010@\u001a\u00020\u00192\u0006\u0010A\u001a\u00020\u0019J\b\u0010B\u001a\u00020\u001bH\u0002J\b\u0010C\u001a\u00020\u001bH\u0002J\u000e\u0010D\u001a\u00020\u001b2\u0006\u00109\u001a\u00020\u0019J\b\u0010E\u001a\u00020\u001bH\u0002J\b\u0010F\u001a\u00020\u001bH\u0002J\u0006\u0010G\u001a\u00020\u001bJ\u0006\u0010H\u001a\u00020\u001bJ\u000e\u0010I\u001a\u00020\u001b2\u0006\u0010J\u001a\u00020KJ\u0010\u0010L\u001a\u00020\u001b2\u0006\u0010M\u001a\u00020\u0019H\u0002J\u0006\u0010N\u001a\u00020\u001bR\u0014\u0010\u0007\u001a\b\u0012\u0004\u0012\u00020\t0\bX\u0082\u0004¢\u0006\u0002\n\u0000R\u0010\u0010\u0004\u001a\u0004\u0018\u00010\u0005X\u0082\u0004¢\u0006\u0002\n\u0000R\u0010\u0010\n\u001a\u0004\u0018\u00010\u000bX\u0082\u000e¢\u0006\u0002\n\u0000R\u000e\u0010\f\u001a\u00020\rX\u0082.¢\u0006\u0002\n\u0000R\u0014\u0010\u000e\u001a\b\u0012\u0004\u0012\u00020\u000f0\bX\u0082\u0004¢\u0006\u0002\n\u0000R\u0010\u0010\u0010\u001a\u0004\u0018\u00010\u0011X\u0082\u000e¢\u0006\u0002\n\u0000R\u0010\u0010\u0012\u001a\u0004\u0018\u00010\u0013X\u0082\u000e¢\u0006\u0002\n\u0000RF\u0010\u0014\u001a:\u0012\u0013\u0012\u00110\t¢\u0006\f\b\u0016\u0012\b\b\u0017\u0012\u0004\b\b(\u0018\u0012\u0013\u0012\u00110\u0019¢\u0006\f\b\u0016\u0012\b\b\u0017\u0012\u0004\b\b(\u001a\u0012\u0004\u0012\u00020\u001b\u0018\u00010\u0015j\u0004\u0018\u0001`\u001cX\u0082\u000e¢\u0006\u0002\n\u0000R\u000e\u0010\u001d\u001a\u00020\u0019X\u0082\u000e¢\u0006\u0002\n\u0000R\u000e\u0010\u001e\u001a\u00020\u0019X\u0082\u000e¢\u0006\u0002\n\u0000R\u000e\u0010\u001f\u001a\u00020\u0019X\u0082\u000e¢\u0006\u0002\n\u0000R\u0010\u0010\u0002\u001a\u0004\u0018\u00010\u0003X\u0082\u0004¢\u0006\u0002\n\u0000R\u000e\u0010 \u001a\u00020\u0019X\u0082\u000e¢\u0006\u0002\n\u0000R\u000e\u0010!\u001a\u00020\"X\u0082\u000e¢\u0006\u0002\n\u0000R\u000e\u0010#\u001a\u00020\u0019X\u0082\u000e¢\u0006\u0002\n\u0000R[\u0010$\u001aO\u0012\u0013\u0012\u00110\u000f¢\u0006\f\b\u0016\u0012\b\b\u0017\u0012\u0004\b\b(&\u0012\u0013\u0012\u00110\u0019¢\u0006\f\b\u0016\u0012\b\b\u0017\u0012\u0004\b\b('\u0012\u0013\u0012\u00110(¢\u0006\f\b\u0016\u0012\b\b\u0017\u0012\u0004\b\b()\u0012\u0004\u0012\u00020\u001b\u0018\u00010%j\u0004\u0018\u0001`*X\u0082\u000e¢\u0006\u0002\n\u0000¨\u0006O"}, d2 = {"Lcom/blackhub/bronline/game/gui/familysystem/UIMenuCarPark;", "Lcom/blackhub/bronline/game/common/UILayout;", "mainActivity", "Lcom/blackhub/bronline/game/core/JNIActivity;", "actionsWithJSON", "Lcom/blackhub/bronline/game/gui/familysystem/network/ActionsWithJSON;", "(Lcom/blackhub/bronline/game/core/JNIActivity;Lcom/blackhub/bronline/game/gui/familysystem/network/ActionsWithJSON;)V", "actionsList", "", "Lcom/blackhub/bronline/game/gui/familysystem/data/FamilyPlayersAction;", "anim", "Landroid/view/animation/Animation;", "binding", "Lcom/blackhub/bronline/databinding/FamiliesMenuCarParkLayoutBinding;", "carList", "Lcom/blackhub/bronline/game/gui/familysystem/data/FamilyMainItemData;", "carsActionAdapter", "Lcom/blackhub/bronline/game/gui/familysystem/adapters/FamilyPlayersActionAdapter;", "carsAdapter", "Lcom/blackhub/bronline/game/gui/familysystem/adapters/FamilyMenuMainAdapter;", "clickActionsButton", "Lkotlin/Function2;", "Lkotlin/ParameterName;", "name", "thisAction", "", "action", "", "Lcom/blackhub/bronline/game/gui/familysystem/OnClickActionsButton;", "currentAction", "currentCarsId", "garage", "oldActionsId", "oldTimerForClick", "", "savedRank", "setClickCar", "Lkotlin/Function3;", "item", "position", "Landroid/view/View;", "mView", "Lcom/blackhub/bronline/game/gui/familysystem/OnClickMenuMainList;", "deleteCar", "carId", "getCarsInfo", "currentCar", "currentPosition", "getView", "onCreateView", "inflater", "Landroid/view/LayoutInflater;", TtmlNode.RUBY_CONTAINER, "Landroid/view/ViewGroup;", "onLayoutClose", "onLayoutShown", "setActionsIfDownload", "garageStatus", "setActionsIfUnload", "setCarsList", "carsId", "carsName", "", "setCurrentRankAndStatus", "carsRank", "carsStatus", "setDataWithActionsInView", "setDataWithCarsListInView", "setGarage", "setLogicForActionClick", "setLogicForCarClick", "setNullableParameters", "setStartData", "setVisibleButtonBackToMenu", "ifFromMainMenu", "", "setVisibleForCarInfo", "isVisible", "updateStatusAfterAction", "app_siteRelease"}, k = 1, mv = {1, 9, 0}, xi = 48)
/* loaded from: classes3.dex */
public final class UIMenuCarPark extends UILayout {
    public static final int $stable = 8;

    @Nullable
    public final ActionsWithJSON actionsWithJSON;

    @Nullable
    public Animation anim;
    public FamiliesMenuCarParkLayoutBinding binding;

    @Nullable
    public FamilyPlayersActionAdapter carsActionAdapter;

    @Nullable
    public FamilyMenuMainAdapter carsAdapter;

    @Nullable
    public Function2<? super FamilyPlayersAction, ? super Integer, Unit> clickActionsButton;

    @Nullable
    public final JNIActivity mainActivity;
    public long oldTimerForClick;
    public int savedRank;

    @Nullable
    public Function3<? super FamilyMainItemData, ? super Integer, ? super View, Unit> setClickCar;

    @NotNull
    public final List<FamilyMainItemData> carList = new ArrayList();

    @NotNull
    public final List<FamilyPlayersAction> actionsList = new ArrayList();
    public int currentCarsId = -1;
    public int currentAction = -1;
    public int oldActionsId = -1;
    public int garage = -1;

    @Override // com.blackhub.bronline.game.common.UILayout
    public void onLayoutClose() {
    }

    @Override // com.blackhub.bronline.game.common.UILayout
    public void onLayoutShown() {
    }

    public UIMenuCarPark(@Nullable JNIActivity jNIActivity, @Nullable ActionsWithJSON actionsWithJSON) {
        this.mainActivity = jNIActivity;
        this.actionsWithJSON = actionsWithJSON;
    }

    public final void setVisibleButtonBackToMenu(boolean ifFromMainMenu) {
        FamiliesMenuCarParkLayoutBinding familiesMenuCarParkLayoutBinding = this.binding;
        if (familiesMenuCarParkLayoutBinding == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            familiesMenuCarParkLayoutBinding = null;
        }
        familiesMenuCarParkLayoutBinding.buttonBackToMenu.setVisibility(ifFromMainMenu ? 4 : 0);
    }

    public final void setStartData() {
        this.currentCarsId = -1;
        this.currentAction = -1;
        this.oldActionsId = -1;
        this.savedRank = 0;
        if (this.carList.size() != 0) {
            getCarsInfo(this.carList.get(0), 0);
            setVisibleForCarInfo(0);
        } else {
            setVisibleForCarInfo(4);
        }
    }

    public final void setVisibleForCarInfo(int isVisible) {
        FamiliesMenuCarParkLayoutBinding familiesMenuCarParkLayoutBinding = this.binding;
        if (familiesMenuCarParkLayoutBinding == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            familiesMenuCarParkLayoutBinding = null;
        }
        familiesMenuCarParkLayoutBinding.actionWithCar.setVisibility(isVisible);
    }

    @Override // com.blackhub.bronline.game.common.UILayout
    @NotNull
    /* renamed from: getView */
    public View getMViewRoot() {
        FamiliesMenuCarParkLayoutBinding familiesMenuCarParkLayoutBinding = this.binding;
        if (familiesMenuCarParkLayoutBinding == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            familiesMenuCarParkLayoutBinding = null;
        }
        ConstraintLayout root = familiesMenuCarParkLayoutBinding.getRoot();
        Intrinsics.checkNotNullExpressionValue(root, "getRoot(...)");
        return root;
    }

    @Override // com.blackhub.bronline.game.common.UILayout
    @NotNull
    public View onCreateView(@NotNull LayoutInflater inflater, @Nullable ViewGroup r4) {
        Intrinsics.checkNotNullParameter(inflater, "inflater");
        FamiliesMenuCarParkLayoutBinding inflate = FamiliesMenuCarParkLayoutBinding.inflate(inflater);
        Intrinsics.checkNotNullExpressionValue(inflate, "inflate(...)");
        this.binding = inflate;
        FamiliesMenuCarParkLayoutBinding familiesMenuCarParkLayoutBinding = null;
        if (inflate == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            inflate = null;
        }
        this.anim = AnimationUtils.loadAnimation(inflate.getRoot().getContext(), R.anim.button_click);
        setLogicForCarClick();
        setDataWithCarsListInView();
        setLogicForActionClick();
        setDataWithActionsInView();
        FamiliesMenuCarParkLayoutBinding familiesMenuCarParkLayoutBinding2 = this.binding;
        if (familiesMenuCarParkLayoutBinding2 == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            familiesMenuCarParkLayoutBinding2 = null;
        }
        familiesMenuCarParkLayoutBinding2.buttonBackToMenu.setOnClickListener(new View.OnClickListener() { // from class: com.blackhub.bronline.game.gui.familysystem.UIMenuCarPark$$ExternalSyntheticLambda0
            public /* synthetic */ UIMenuCarPark$$ExternalSyntheticLambda0() {
            }

            @Override // android.view.View.OnClickListener
            public final void onClick(View view) {
                UIMenuCarPark.onCreateView$lambda$3$lambda$2(UIMenuCarPark.this, view);
            }
        });
        FamiliesMenuCarParkLayoutBinding familiesMenuCarParkLayoutBinding3 = this.binding;
        if (familiesMenuCarParkLayoutBinding3 == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
        } else {
            familiesMenuCarParkLayoutBinding = familiesMenuCarParkLayoutBinding3;
        }
        ConstraintLayout root = familiesMenuCarParkLayoutBinding.getRoot();
        Intrinsics.checkNotNullExpressionValue(root, "getRoot(...)");
        return root;
    }

    public static final void onCreateView$lambda$3$lambda$2(UIMenuCarPark this$0, View view) {
        Intrinsics.checkNotNullParameter(this$0, "this$0");
        ActionsWithJSON actionsWithJSON = this$0.actionsWithJSON;
        if (actionsWithJSON != null) {
            actionsWithJSON.sendGetLayout(5);
        }
    }

    public final void setLogicForCarClick() {
        this.setClickCar = new Function3<FamilyMainItemData, Integer, View, Unit>() { // from class: com.blackhub.bronline.game.gui.familysystem.UIMenuCarPark$setLogicForCarClick$1
            public UIMenuCarPark$setLogicForCarClick$1() {
            }

            @Override // kotlin.jvm.functions.Function3
            public /* bridge */ /* synthetic */ Unit invoke(FamilyMainItemData familyMainItemData, Integer num, View view) {
                invoke(familyMainItemData, num.intValue(), view);
                return Unit.INSTANCE;
            }

            public void invoke(@NotNull FamilyMainItemData item, int position, @NotNull View mView) {
                long j;
                Animation animation;
                FamiliesMenuCarParkLayoutBinding familiesMenuCarParkLayoutBinding;
                Intrinsics.checkNotNullParameter(item, "item");
                Intrinsics.checkNotNullParameter(mView, "mView");
                long currentTimeMillis = System.currentTimeMillis();
                j = UIMenuCarPark.this.oldTimerForClick;
                if (currentTimeMillis - j > 700) {
                    UIMenuCarPark.this.oldTimerForClick = System.currentTimeMillis();
                    animation = UIMenuCarPark.this.anim;
                    mView.startAnimation(animation);
                    UIMenuCarPark.this.getCarsInfo(item, position);
                    familiesMenuCarParkLayoutBinding = UIMenuCarPark.this.binding;
                    if (familiesMenuCarParkLayoutBinding == null) {
                        Intrinsics.throwUninitializedPropertyAccessException("binding");
                        familiesMenuCarParkLayoutBinding = null;
                    }
                    if (familiesMenuCarParkLayoutBinding.actionWithCar.getVisibility() == 4) {
                        UIMenuCarPark.this.setVisibleForCarInfo(0);
                    }
                }
            }
        };
    }

    public final void setDataWithCarsListInView() {
        this.carsAdapter = new FamilyMenuMainAdapter(this.carList, this.setClickCar, this.mainActivity);
        FamiliesMenuCarParkLayoutBinding familiesMenuCarParkLayoutBinding = this.binding;
        FamiliesMenuCarParkLayoutBinding familiesMenuCarParkLayoutBinding2 = null;
        if (familiesMenuCarParkLayoutBinding == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            familiesMenuCarParkLayoutBinding = null;
        }
        RecyclerView recyclerView = familiesMenuCarParkLayoutBinding.carList;
        FamiliesMenuCarParkLayoutBinding familiesMenuCarParkLayoutBinding3 = this.binding;
        if (familiesMenuCarParkLayoutBinding3 == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
        } else {
            familiesMenuCarParkLayoutBinding2 = familiesMenuCarParkLayoutBinding3;
        }
        recyclerView.setLayoutManager(new LinearLayoutManager(familiesMenuCarParkLayoutBinding2.getRoot().getContext(), 1, false));
        recyclerView.setAdapter(this.carsAdapter);
    }

    public final void getCarsInfo(FamilyMainItemData currentCar, int currentPosition) {
        this.currentCarsId = currentCar.getItemsId();
        currentCar.setClicked(true);
        FamilyMenuMainAdapter familyMenuMainAdapter = this.carsAdapter;
        if (familyMenuMainAdapter != null) {
            familyMenuMainAdapter.notifyItemChanged(currentPosition);
            familyMenuMainAdapter.setCheckOnlyElement(currentPosition);
        }
        ActionsWithJSON actionsWithJSON = this.actionsWithJSON;
        if (actionsWithJSON != null) {
            actionsWithJSON.clickOnCar(currentCar.getItemsId());
        }
    }

    public final void setLogicForActionClick() {
        this.clickActionsButton = new Function2<FamilyPlayersAction, Integer, Unit>() { // from class: com.blackhub.bronline.game.gui.familysystem.UIMenuCarPark$setLogicForActionClick$1
            public UIMenuCarPark$setLogicForActionClick$1() {
            }

            @Override // kotlin.jvm.functions.Function2
            public /* bridge */ /* synthetic */ Unit invoke(FamilyPlayersAction familyPlayersAction, Integer num) {
                invoke(familyPlayersAction, num.intValue());
                return Unit.INSTANCE;
            }

            /* JADX WARN: Code restructure failed: missing block: B:10:0x003b, code lost:
            
                r6 = com.blackhub.bronline.game.gui.familysystem.UIMenuCarPark.this.actionsWithJSON;
             */
            /*
                Code decompiled incorrectly, please refer to instructions dump.
                To view partially-correct code enable 'Show inconsistent code' option in preferences
            */
            public void invoke(@org.jetbrains.annotations.NotNull com.blackhub.bronline.game.gui.familysystem.data.FamilyPlayersAction r5, int r6) {
                /*
                    r4 = this;
                    java.lang.String r0 = "thisAction"
                    kotlin.jvm.internal.Intrinsics.checkNotNullParameter(r5, r0)
                    long r0 = java.lang.System.currentTimeMillis()
                    com.blackhub.bronline.game.gui.familysystem.UIMenuCarPark r2 = com.blackhub.bronline.game.gui.familysystem.UIMenuCarPark.this
                    long r2 = com.blackhub.bronline.game.gui.familysystem.UIMenuCarPark.access$getOldTimerForClick$p(r2)
                    long r0 = r0 - r2
                    r2 = 700(0x2bc, double:3.46E-321)
                    int r0 = (r0 > r2 ? 1 : (r0 == r2 ? 0 : -1))
                    if (r0 <= 0) goto L87
                    com.blackhub.bronline.game.gui.familysystem.UIMenuCarPark r0 = com.blackhub.bronline.game.gui.familysystem.UIMenuCarPark.this
                    long r1 = java.lang.System.currentTimeMillis()
                    com.blackhub.bronline.game.gui.familysystem.UIMenuCarPark.access$setOldTimerForClick$p(r0, r1)
                    com.blackhub.bronline.game.gui.familysystem.UIMenuCarPark r0 = com.blackhub.bronline.game.gui.familysystem.UIMenuCarPark.this
                    int r1 = r5.getActionsId()
                    com.blackhub.bronline.game.gui.familysystem.UIMenuCarPark.access$setOldActionsId$p(r0, r1)
                    com.blackhub.bronline.game.gui.familysystem.UIMenuCarPark r0 = com.blackhub.bronline.game.gui.familysystem.UIMenuCarPark.this
                    int r0 = com.blackhub.bronline.game.gui.familysystem.UIMenuCarPark.access$getCurrentCarsId$p(r0)
                    r1 = -1
                    if (r0 == r1) goto L87
                    if (r6 == 0) goto L6c
                    r0 = 1
                    if (r6 == r0) goto L51
                    r0 = 3
                    if (r6 == r0) goto L3b
                    goto L87
                L3b:
                    com.blackhub.bronline.game.gui.familysystem.UIMenuCarPark r6 = com.blackhub.bronline.game.gui.familysystem.UIMenuCarPark.this
                    com.blackhub.bronline.game.gui.familysystem.network.ActionsWithJSON r6 = com.blackhub.bronline.game.gui.familysystem.UIMenuCarPark.access$getActionsWithJSON$p(r6)
                    if (r6 == 0) goto L87
                    com.blackhub.bronline.game.gui.familysystem.UIMenuCarPark r0 = com.blackhub.bronline.game.gui.familysystem.UIMenuCarPark.this
                    int r0 = com.blackhub.bronline.game.gui.familysystem.UIMenuCarPark.access$getCurrentCarsId$p(r0)
                    int r5 = r5.getActionsId()
                    r6.setActionWithCar(r0, r5, r1)
                    goto L87
                L51:
                    com.blackhub.bronline.game.gui.familysystem.UIMenuCarPark r6 = com.blackhub.bronline.game.gui.familysystem.UIMenuCarPark.this
                    com.blackhub.bronline.game.gui.familysystem.network.ActionsWithJSON r6 = com.blackhub.bronline.game.gui.familysystem.UIMenuCarPark.access$getActionsWithJSON$p(r6)
                    if (r6 == 0) goto L66
                    com.blackhub.bronline.game.gui.familysystem.UIMenuCarPark r1 = com.blackhub.bronline.game.gui.familysystem.UIMenuCarPark.this
                    int r1 = com.blackhub.bronline.game.gui.familysystem.UIMenuCarPark.access$getCurrentCarsId$p(r1)
                    int r5 = r5.getActionsId()
                    r6.setActionWithCar(r1, r5, r0)
                L66:
                    com.blackhub.bronline.game.gui.familysystem.UIMenuCarPark r5 = com.blackhub.bronline.game.gui.familysystem.UIMenuCarPark.this
                    com.blackhub.bronline.game.gui.familysystem.UIMenuCarPark.access$setCurrentAction$p(r5, r0)
                    goto L87
                L6c:
                    com.blackhub.bronline.game.gui.familysystem.UIMenuCarPark r6 = com.blackhub.bronline.game.gui.familysystem.UIMenuCarPark.this
                    com.blackhub.bronline.game.gui.familysystem.network.ActionsWithJSON r6 = com.blackhub.bronline.game.gui.familysystem.UIMenuCarPark.access$getActionsWithJSON$p(r6)
                    r0 = 0
                    if (r6 == 0) goto L82
                    com.blackhub.bronline.game.gui.familysystem.UIMenuCarPark r1 = com.blackhub.bronline.game.gui.familysystem.UIMenuCarPark.this
                    int r1 = com.blackhub.bronline.game.gui.familysystem.UIMenuCarPark.access$getCurrentCarsId$p(r1)
                    int r5 = r5.getActionsId()
                    r6.setActionWithCar(r1, r5, r0)
                L82:
                    com.blackhub.bronline.game.gui.familysystem.UIMenuCarPark r5 = com.blackhub.bronline.game.gui.familysystem.UIMenuCarPark.this
                    com.blackhub.bronline.game.gui.familysystem.UIMenuCarPark.access$setCurrentAction$p(r5, r0)
                L87:
                    return
                */
                throw new UnsupportedOperationException("Method not decompiled: com.blackhub.bronline.game.gui.familysystem.UIMenuCarPark$setLogicForActionClick$1.invoke(com.blackhub.bronline.game.gui.familysystem.data.FamilyPlayersAction, int):void");
            }
        };
    }

    public final void setDataWithActionsInView() {
        this.carsActionAdapter = new FamilyPlayersActionAdapter(this.actionsList, this.clickActionsButton);
        FamiliesMenuCarParkLayoutBinding familiesMenuCarParkLayoutBinding = this.binding;
        FamiliesMenuCarParkLayoutBinding familiesMenuCarParkLayoutBinding2 = null;
        if (familiesMenuCarParkLayoutBinding == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            familiesMenuCarParkLayoutBinding = null;
        }
        RecyclerView recyclerView = familiesMenuCarParkLayoutBinding.actionWithCar;
        FamiliesMenuCarParkLayoutBinding familiesMenuCarParkLayoutBinding3 = this.binding;
        if (familiesMenuCarParkLayoutBinding3 == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
        } else {
            familiesMenuCarParkLayoutBinding2 = familiesMenuCarParkLayoutBinding3;
        }
        recyclerView.setLayoutManager(new LinearLayoutManager(familiesMenuCarParkLayoutBinding2.getRoot().getContext(), 1, false));
        recyclerView.setAdapter(this.carsActionAdapter);
    }

    @SuppressLint({"NotifyDataSetChanged"})
    public final void setCarsList(@NotNull List<Integer> carsId, @NotNull List<String> carsName) {
        Intrinsics.checkNotNullParameter(carsId, "carsId");
        Intrinsics.checkNotNullParameter(carsName, "carsName");
        if (carsId.size() == carsName.size()) {
            this.carList.clear();
            int size = carsId.size();
            for (int i = 0; i < size; i++) {
                this.carList.add(new FamilyMainItemData(carsId.get(i).intValue(), carsName.get(i), false, 4, null));
            }
            FamilyMenuMainAdapter familyMenuMainAdapter = this.carsAdapter;
            if (familyMenuMainAdapter != null) {
                familyMenuMainAdapter.notifyDataSetChanged();
            }
        }
    }

    public final void setGarage(int garageStatus) {
        this.garage = garageStatus;
    }

    @SuppressLint({"NotifyDataSetChanged"})
    public final void setActionsIfDownload(int garageStatus) {
        List<FamilyPlayersAction> list = this.actionsList;
        list.clear();
        list.add(new FamilyPlayersAction(0, "Отметить на GPS", 0, 2));
        list.add(new FamilyPlayersAction(1, "Загрузить транспорт на парковке", 0, 2));
        list.add(new FamilyPlayersAction(6, "Загрузить транспорт на месте выгрузки", 0, 2));
        if (garageStatus == 1) {
            list.add(new FamilyPlayersAction(5, "Загрузить транспорт в гараж", 0, 2));
        }
        list.add(new FamilyPlayersAction(2, "Вернуть из семьи", 0, 2));
        list.add(new FamilyPlayersAction(3, "Выдать доступ", this.savedRank, 0));
        list.add(new FamilyPlayersAction(7, "Сбросить парковку", 0, 2));
        FamilyPlayersActionAdapter familyPlayersActionAdapter = this.carsActionAdapter;
        if (familyPlayersActionAdapter != null) {
            familyPlayersActionAdapter.notifyDataSetChanged();
        }
    }

    @SuppressLint({"NotifyDataSetChanged"})
    public final void setActionsIfUnload() {
        List<FamilyPlayersAction> list = this.actionsList;
        list.clear();
        list.add(new FamilyPlayersAction(0, "Отметить на GPS", 0, 2));
        list.add(new FamilyPlayersAction(4, "Выгрузить транспорт", 0, 2));
        list.add(new FamilyPlayersAction(2, "Вернуть из семьи", 0, 2));
        list.add(new FamilyPlayersAction(3, "Выдать доступ", this.savedRank, 0));
        list.add(new FamilyPlayersAction(7, "Сбросить парковку", 0, 2));
        FamilyPlayersActionAdapter familyPlayersActionAdapter = this.carsActionAdapter;
        if (familyPlayersActionAdapter != null) {
            familyPlayersActionAdapter.notifyDataSetChanged();
        }
    }

    public final void setCurrentRankAndStatus(int carsRank, int carsStatus) {
        if (carsStatus == 0) {
            setActionsIfDownload(this.garage);
        } else if (carsStatus == 1) {
            setActionsIfUnload();
        }
        List<FamilyPlayersAction> list = this.actionsList;
        if (list.size() > 0) {
            int size = list.size();
            for (int i = 0; i < size; i++) {
                if (list.get(i).getActionsId() == 3) {
                    list.get(i).setActionsValue(carsRank);
                    this.savedRank = carsRank;
                    FamilyPlayersActionAdapter familyPlayersActionAdapter = this.carsActionAdapter;
                    if (familyPlayersActionAdapter != null) {
                        familyPlayersActionAdapter.notifyItemChanged(i);
                        return;
                    }
                    return;
                }
            }
        }
    }

    public final void updateStatusAfterAction() {
        List<FamilyPlayersAction> list = this.actionsList;
        if (list.size() > 0) {
            switch (this.oldActionsId) {
                case 1:
                case 5:
                case 6:
                    setActionsIfUnload();
                    break;
                case 2:
                    deleteCar(this.currentCarsId);
                    break;
                case 3:
                    int size = list.size();
                    int i = 0;
                    while (true) {
                        if (i >= size) {
                            break;
                        } else if (list.get(i).getActionsId() == 3) {
                            int i2 = this.currentAction;
                            if (i2 == 0) {
                                FamilyPlayersAction familyPlayersAction = list.get(i);
                                familyPlayersAction.setActionsValue(familyPlayersAction.getActionsValue() - 1);
                                this.savedRank = list.get(i).getActionsValue();
                            } else if (i2 == 1) {
                                FamilyPlayersAction familyPlayersAction2 = list.get(i);
                                familyPlayersAction2.setActionsValue(familyPlayersAction2.getActionsValue() + 1);
                                this.savedRank = list.get(i).getActionsValue();
                            }
                            FamilyPlayersActionAdapter familyPlayersActionAdapter = this.carsActionAdapter;
                            if (familyPlayersActionAdapter != null) {
                                familyPlayersActionAdapter.notifyItemChanged(i);
                                break;
                            }
                        } else {
                            i++;
                        }
                    }
                    break;
                case 4:
                    setActionsIfDownload(this.garage);
                    break;
            }
            this.currentAction = -1;
            this.oldActionsId = -1;
        }
    }

    public final void deleteCar(int carId) {
        if (this.carList.size() > 0) {
            int size = this.carList.size();
            int i = 0;
            while (true) {
                if (i >= size) {
                    break;
                }
                if (this.carList.get(i).getItemsId() == carId) {
                    this.carList.remove(i);
                    FamilyMenuMainAdapter familyMenuMainAdapter = this.carsAdapter;
                    if (familyMenuMainAdapter != null) {
                        familyMenuMainAdapter.notifyItemRemoved(i);
                    }
                } else {
                    i++;
                }
            }
            this.currentCarsId = -1;
            this.currentAction = -1;
            this.oldActionsId = -1;
            setVisibleForCarInfo(4);
        }
    }

    public final void setNullableParameters() {
        this.anim = null;
        this.carsAdapter = null;
        this.setClickCar = null;
        this.carsActionAdapter = null;
        this.clickActionsButton = null;
        this.carList.clear();
        this.actionsList.clear();
    }
}
