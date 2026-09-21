package com.blackhub.bronline.game.gui.familysystem;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.widget.TextView;
import androidx.compose.runtime.internal.StabilityInferred;
import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.core.view.GravityCompat;
import androidx.media3.extractor.text.ttml.TtmlNode;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;
import com.blackhub.bronline.R;
import com.blackhub.bronline.databinding.FamiliesUpgradesLayoutBinding;
import com.blackhub.bronline.game.common.UILayout;
import com.blackhub.bronline.game.common.dotrvindicator.DotViewIndicator;
import com.blackhub.bronline.game.common.gravitysnaphelper.GravitySnapHelper;
import com.blackhub.bronline.game.core.JNIActivity;
import com.blackhub.bronline.game.core.extension.AnyExtensionKt;
import com.blackhub.bronline.game.gui.UsefulKt;
import com.blackhub.bronline.game.gui.familysystem.adapters.FamilyUpgradeAdapter;
import com.blackhub.bronline.game.gui.familysystem.adapters.FamilyUpgradeParentItemAdapter;
import com.blackhub.bronline.game.gui.familysystem.data.FamilySystemList;
import com.blackhub.bronline.game.gui.familysystem.data.FamilySystemUpgradeObj;
import com.blackhub.bronline.game.gui.familysystem.data.FamilyUpgradeData;
import com.blackhub.bronline.game.gui.familysystem.data.FamilyUpgradeValueData;
import com.blackhub.bronline.game.gui.familysystem.network.ActionsWithJSON;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import kotlin.Metadata;
import kotlin.Unit;
import kotlin.jvm.functions.Function0;
import kotlin.jvm.functions.Function2;
import kotlin.jvm.internal.Intrinsics;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

/* compiled from: UIMenuFamilyUpgrade.kt */
@StabilityInferred(parameters = 0)
@Metadata(d1 = {"\u0000¨\u0001\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0002\b\u0002\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0002\b\u0002\n\u0002\u0010\b\n\u0000\n\u0002\u0010\u0002\n\u0002\u0018\u0002\n\u0002\b\u0002\n\u0002\u0018\u0002\n\u0000\n\u0002\u0010!\n\u0002\b\u0006\n\u0002\u0010\t\n\u0002\b\u0002\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0002\b\t\n\u0002\u0010 \n\u0000\n\u0002\u0018\u0002\n\u0002\b\u0002\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0002\b\u0004\n\u0002\u0010\u000e\n\u0002\b\u0011\n\u0002\u0010\u000b\n\u0002\b\u0007\b\u0007\u0018\u00002\u00020\u0001B+\u0012\u0006\u0010\u0002\u001a\u00020\u0003\u0012\b\u0010\u0004\u001a\u0004\u0018\u00010\u0005\u0012\b\u0010\u0006\u001a\u0004\u0018\u00010\u0007\u0012\b\u0010\b\u001a\u0004\u0018\u00010\t¢\u0006\u0002\u0010\nJ,\u0010.\u001a\b\u0012\u0004\u0012\u00020+0\u001d2\f\u0010/\u001a\b\u0012\u0004\u0012\u00020+0\u001d2\u0006\u00100\u001a\u00020\u00152\u0006\u00101\u001a\u00020\u0015H\u0002J\u0010\u00102\u001a\u00020\u00172\u0006\u00103\u001a\u00020\u0015H\u0002J&\u00104\u001a\u00020\u00152\u0006\u00100\u001a\u00020\u00152\u0006\u00101\u001a\u00020\u00152\f\u0010/\u001a\b\u0012\u0004\u0012\u00020)05H\u0002J\b\u00106\u001a\u000207H\u0016J\u001a\u00108\u001a\u0002072\u0006\u00109\u001a\u00020:2\b\u0010;\u001a\u0004\u0018\u00010<H\u0016J\b\u0010=\u001a\u00020\u0017H\u0016J\b\u0010>\u001a\u00020\u0017H\u0016J\u0018\u0010?\u001a\u00020+2\u0006\u0010@\u001a\u00020A2\u0006\u0010B\u001a\u00020\u0015H\u0002J\u0018\u0010C\u001a\u00020\u00172\u0006\u0010D\u001a\u00020\u00152\u0006\u00104\u001a\u00020\u0015H\u0002J\u0018\u0010E\u001a\u00020\u00172\u0006\u0010F\u001a\u00020\u00152\u0006\u0010G\u001a\u00020\u0015H\u0002J\b\u0010H\u001a\u00020\u0017H\u0002J\u0006\u0010I\u001a\u00020\u0017J\u001c\u0010J\u001a\u00020\u00172\u0006\u0010K\u001a\u00020\u00152\f\u0010L\u001a\b\u0012\u0004\u0012\u00020\u00150\u001dJ\b\u0010M\u001a\u00020\u0017H\u0002J\b\u0010N\u001a\u00020\u0017H\u0002J\b\u0010O\u001a\u00020\u0017H\u0002J\u0018\u0010P\u001a\u00020\u00172\u0006\u00100\u001a\u00020\u00152\u0006\u00101\u001a\u00020\u0015H\u0002J\u000e\u0010Q\u001a\u00020\u00172\u0006\u0010R\u001a\u00020SJ\u0010\u0010T\u001a\u00020\u00172\u0006\u0010U\u001a\u00020\u0015H\u0002J\u0010\u0010V\u001a\u00020\u00172\u0006\u0010\u0014\u001a\u00020\u0011H\u0002J\u000e\u0010W\u001a\u00020\u00172\u0006\u00100\u001a\u00020\u0015J\u0010\u0010X\u001a\u00020\u00172\u0006\u0010\u0014\u001a\u00020\u0011H\u0002J\u0006\u0010Y\u001a\u00020\u0017R\u0010\u0010\u0006\u001a\u0004\u0018\u00010\u0007X\u0082\u0004¢\u0006\u0002\n\u0000R\u0010\u0010\u000b\u001a\u0004\u0018\u00010\fX\u0082\u000e¢\u0006\u0002\n\u0000R\u000e\u0010\r\u001a\u00020\u000eX\u0082.¢\u0006\u0002\n\u0000RF\u0010\u000f\u001a:\u0012\u0013\u0012\u00110\u0011¢\u0006\f\b\u0012\u0012\b\b\u0013\u0012\u0004\b\b(\u0014\u0012\u0013\u0012\u00110\u0015¢\u0006\f\b\u0012\u0012\b\b\u0013\u0012\u0004\b\b(\u0016\u0012\u0004\u0012\u00020\u0017\u0018\u00010\u0010j\u0004\u0018\u0001`\u0018X\u0082\u000e¢\u0006\u0002\n\u0000R\u000e\u0010\u0019\u001a\u00020\u0015X\u0082\u000e¢\u0006\u0002\n\u0000R\u0010\u0010\u001a\u001a\u0004\u0018\u00010\u001bX\u0082\u000e¢\u0006\u0002\n\u0000R\u0014\u0010\u001c\u001a\b\u0012\u0004\u0012\u00020\u00150\u001dX\u0082\u0004¢\u0006\u0002\n\u0000R\u0010\u0010\b\u001a\u0004\u0018\u00010\tX\u0082\u0004¢\u0006\u0002\n\u0000R\u0010\u0010\u0004\u001a\u0004\u0018\u00010\u0005X\u0082\u0004¢\u0006\u0002\n\u0000R\u000e\u0010\u0002\u001a\u00020\u0003X\u0082\u0004¢\u0006\u0002\n\u0000R\u000e\u0010\u001e\u001a\u00020\u0015X\u0082\u000e¢\u0006\u0002\n\u0000R\u000e\u0010\u001f\u001a\u00020\u0015X\u0082\u000e¢\u0006\u0002\n\u0000R\u000e\u0010 \u001a\u00020\u0015X\u0082\u000e¢\u0006\u0002\n\u0000R\u000e\u0010!\u001a\u00020\u0015X\u0082\u000e¢\u0006\u0002\n\u0000R\u000e\u0010\"\u001a\u00020\u0015X\u0082\u000e¢\u0006\u0002\n\u0000R\u000e\u0010#\u001a\u00020$X\u0082\u000e¢\u0006\u0002\n\u0000R\u000e\u0010%\u001a\u00020\u0015X\u0082\u000e¢\u0006\u0002\n\u0000R\u0010\u0010&\u001a\u0004\u0018\u00010'X\u0082\u000e¢\u0006\u0002\n\u0000R\u0014\u0010(\u001a\b\u0012\u0004\u0012\u00020)0\u001dX\u0082\u0004¢\u0006\u0002\n\u0000R\u0014\u0010*\u001a\b\u0012\u0004\u0012\u00020+0\u001dX\u0082\u0004¢\u0006\u0002\n\u0000R\u0014\u0010,\u001a\b\u0012\u0004\u0012\u00020\u00110\u001dX\u0082\u0004¢\u0006\u0002\n\u0000R\u0014\u0010-\u001a\b\u0012\u0004\u0012\u00020+0\u001dX\u0082\u0004¢\u0006\u0002\n\u0000¨\u0006Z"}, d2 = {"Lcom/blackhub/bronline/game/gui/familysystem/UIMenuFamilyUpgrade;", "Lcom/blackhub/bronline/game/common/UILayout;", "mainRoot", "Lcom/blackhub/bronline/game/gui/familysystem/GUIFamilySystem;", "mainActivity", "Lcom/blackhub/bronline/game/core/JNIActivity;", "actionsWithJSON", "Lcom/blackhub/bronline/game/gui/familysystem/network/ActionsWithJSON;", "logicForDialogApply", "Lcom/blackhub/bronline/game/gui/familysystem/LogicForDialogApply;", "(Lcom/blackhub/bronline/game/gui/familysystem/GUIFamilySystem;Lcom/blackhub/bronline/game/core/JNIActivity;Lcom/blackhub/bronline/game/gui/familysystem/network/ActionsWithJSON;Lcom/blackhub/bronline/game/gui/familysystem/LogicForDialogApply;)V", "anim", "Landroid/view/animation/Animation;", "binding", "Lcom/blackhub/bronline/databinding/FamiliesUpgradesLayoutBinding;", "clickUpdateItem", "Lkotlin/Function2;", "Lcom/blackhub/bronline/game/gui/familysystem/data/FamilyUpgradeData;", "Lkotlin/ParameterName;", "name", "updateItem", "", "position", "", "Lcom/blackhub/bronline/game/gui/familysystem/OnClickUpdateItem;", "currentUpgradeId", "familyUpgradeParentItemAdapter", "Lcom/blackhub/bronline/game/gui/familysystem/adapters/FamilyUpgradeParentItemAdapter;", "levels", "", "maxFamilyLevel", "maxMoney", "maxStoreLevel", "maxWeaponsLevel", "oldMoney", "oldTimerForClickOnUpgrade", "", "thisPrice", "upgradeAdapter", "Lcom/blackhub/bronline/game/gui/familysystem/adapters/FamilyUpgradeAdapter;", "upgradeAllListFromApp", "Lcom/blackhub/bronline/game/gui/familysystem/data/FamilySystemUpgradeObj;", "upgradeCurrentValueList", "Lcom/blackhub/bronline/game/gui/familysystem/data/FamilyUpgradeValueData;", "upgradeList", "upgradeNewValueList", "actionWithCollection", "currentCollection", "currentId", "currentLevel", "clearInfoAboutUpgrade", "setVisible", "getMaxMoney", "", "getView", "Landroid/view/View;", "onCreateView", "inflater", "Landroid/view/LayoutInflater;", TtmlNode.RUBY_CONTAINER, "Landroid/view/ViewGroup;", "onLayoutClose", "onLayoutShown", "setCurrentData", "description", "", "value", "setCurrentMoney", "getCurrentMoney", "setCurrentUpgradePrice", "thisId", "thisLevel", "setLogicForClickUpdateItem", "setNullableParameters", "setStartData", "currentMoneyOnStore", "currentUpgrades", "setUpdateDataList", "setUpgradeData", "setUpgradeDataInView", "setUpgradeLevels", "setVisibleButtonBackToMenu", "ifFromMainMenu", "", "setVisibleButtonUpgrade", "getPrice", "updateDataForUpgrade", "updateState", "updateTitleForRightBlock", "upgradeFamily", "app_siteRelease"}, k = 1, mv = {1, 9, 0}, xi = 48)
/* loaded from: classes3.dex */
public final class UIMenuFamilyUpgrade extends UILayout {
    public static final int $stable = 8;

    @Nullable
    public final ActionsWithJSON actionsWithJSON;

    @Nullable
    public Animation anim;
    public FamiliesUpgradesLayoutBinding binding;

    @Nullable
    public Function2<? super FamilyUpgradeData, ? super Integer, Unit> clickUpdateItem;
    public int currentUpgradeId;

    @Nullable
    public FamilyUpgradeParentItemAdapter familyUpgradeParentItemAdapter;

    @NotNull
    public final List<Integer> levels;

    @Nullable
    public final LogicForDialogApply logicForDialogApply;

    @Nullable
    public final JNIActivity mainActivity;

    @NotNull
    public final GUIFamilySystem mainRoot;
    public int maxFamilyLevel;
    public int maxMoney;
    public int maxStoreLevel;
    public int maxWeaponsLevel;
    public int oldMoney;
    public long oldTimerForClickOnUpgrade;
    public int thisPrice;

    @Nullable
    public FamilyUpgradeAdapter upgradeAdapter;

    @NotNull
    public final List<FamilySystemUpgradeObj> upgradeAllListFromApp;

    @NotNull
    public final List<FamilyUpgradeValueData> upgradeCurrentValueList;

    @NotNull
    public final List<FamilyUpgradeData> upgradeList;

    @NotNull
    public final List<FamilyUpgradeValueData> upgradeNewValueList;

    @Override // com.blackhub.bronline.game.common.UILayout
    public void onLayoutClose() {
    }

    @Override // com.blackhub.bronline.game.common.UILayout
    public void onLayoutShown() {
    }

    public UIMenuFamilyUpgrade(@NotNull GUIFamilySystem mainRoot, @Nullable JNIActivity jNIActivity, @Nullable ActionsWithJSON actionsWithJSON, @Nullable LogicForDialogApply logicForDialogApply) {
        Intrinsics.checkNotNullParameter(mainRoot, "mainRoot");
        this.mainRoot = mainRoot;
        this.mainActivity = jNIActivity;
        this.actionsWithJSON = actionsWithJSON;
        this.logicForDialogApply = logicForDialogApply;
        this.upgradeList = new ArrayList();
        this.currentUpgradeId = -1;
        this.oldMoney = -1;
        this.maxMoney = -1;
        this.upgradeAllListFromApp = new ArrayList();
        this.upgradeCurrentValueList = new ArrayList();
        this.upgradeNewValueList = new ArrayList();
        this.levels = new ArrayList();
    }

    public final void setVisibleButtonBackToMenu(boolean ifFromMainMenu) {
        FamiliesUpgradesLayoutBinding familiesUpgradesLayoutBinding = this.binding;
        if (familiesUpgradesLayoutBinding == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            familiesUpgradesLayoutBinding = null;
        }
        familiesUpgradesLayoutBinding.buttonBackToMenu.setVisibility(ifFromMainMenu ? 4 : 0);
    }

    public final void setStartData(int currentMoneyOnStore, @NotNull List<Integer> currentUpgrades) {
        Intrinsics.checkNotNullParameter(currentUpgrades, "currentUpgrades");
        this.currentUpgradeId = -1;
        this.thisPrice = 0;
        this.oldMoney = currentMoneyOnStore;
        if (this.upgradeList.size() > 0 && this.upgradeList.size() == currentUpgrades.size()) {
            int size = this.upgradeList.size();
            for (int i = 0; i < size; i++) {
                FamilyUpgradeData familyUpgradeData = this.upgradeList.get(i);
                familyUpgradeData.setClicked(false);
                familyUpgradeData.setUpgradeCurrentLevel(currentUpgrades.get(i).intValue());
                if (i == 0) {
                    this.maxMoney = getMaxMoney(familyUpgradeData.getUpgradeId(), familyUpgradeData.getUpgradeCurrentLevel(), this.upgradeAllListFromApp);
                }
                FamilyUpgradeAdapter familyUpgradeAdapter = this.upgradeAdapter;
                if (familyUpgradeAdapter != null) {
                    familyUpgradeAdapter.notifyItemChanged(i);
                }
            }
        }
        setCurrentMoney(this.oldMoney, this.maxMoney);
        clearInfoAboutUpgrade(4);
    }

    public final void setCurrentMoney(int getCurrentMoney, int getMaxMoney) {
        FamiliesUpgradesLayoutBinding familiesUpgradesLayoutBinding = this.binding;
        if (familiesUpgradesLayoutBinding == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            familiesUpgradesLayoutBinding = null;
        }
        TextView textView = familiesUpgradesLayoutBinding.moneyOnStoreValue;
        JNIActivity jNIActivity = this.mainActivity;
        textView.setText(jNIActivity != null ? jNIActivity.getString(R.string.families_info_money, UsefulKt.getPriceWithSpaces(Integer.valueOf(getCurrentMoney)), UsefulKt.getPriceWithSpaces(Integer.valueOf(getMaxMoney))) : null);
        textView.setSelected(true);
    }

    public final int getMaxMoney(int currentId, int currentLevel, List<FamilySystemUpgradeObj> currentCollection) {
        for (FamilySystemUpgradeObj familySystemUpgradeObj : currentCollection) {
            if (familySystemUpgradeObj.getId() == currentId && familySystemUpgradeObj.getLevel() == currentLevel) {
                return familySystemUpgradeObj.getMoney();
            }
        }
        return 0;
    }

    public final void updateState(int currentId) {
        int size = this.upgradeList.size();
        for (int i = 0; i < size; i++) {
            if (this.upgradeList.get(i).getUpgradeId() == currentId) {
                FamilyUpgradeData familyUpgradeData = this.upgradeList.get(i);
                familyUpgradeData.setUpgradeCurrentLevel(familyUpgradeData.getUpgradeCurrentLevel() + 1);
                FamilyUpgradeAdapter familyUpgradeAdapter = this.upgradeAdapter;
                if (familyUpgradeAdapter != null) {
                    familyUpgradeAdapter.notifyItemChanged(i);
                }
                this.oldMoney -= this.thisPrice;
                if (currentId == 0) {
                    this.maxMoney = getMaxMoney(currentId, this.upgradeList.get(i).getUpgradeCurrentLevel(), this.upgradeAllListFromApp);
                }
                setCurrentMoney(this.oldMoney, this.maxMoney);
                updateDataForUpgrade(this.upgradeList.get(i));
                return;
            }
        }
    }

    @Override // com.blackhub.bronline.game.common.UILayout
    @NotNull
    /* renamed from: getView */
    public View getMViewRoot() {
        FamiliesUpgradesLayoutBinding familiesUpgradesLayoutBinding = this.binding;
        if (familiesUpgradesLayoutBinding == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            familiesUpgradesLayoutBinding = null;
        }
        ConstraintLayout root = familiesUpgradesLayoutBinding.getRoot();
        Intrinsics.checkNotNullExpressionValue(root, "getRoot(...)");
        return root;
    }

    @Override // com.blackhub.bronline.game.common.UILayout
    @NotNull
    public View onCreateView(@NotNull LayoutInflater inflater, @Nullable ViewGroup r4) {
        Intrinsics.checkNotNullParameter(inflater, "inflater");
        FamiliesUpgradesLayoutBinding inflate = FamiliesUpgradesLayoutBinding.inflate(inflater);
        Intrinsics.checkNotNullExpressionValue(inflate, "inflate(...)");
        this.binding = inflate;
        FamiliesUpgradesLayoutBinding familiesUpgradesLayoutBinding = null;
        if (inflate == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            inflate = null;
        }
        this.anim = AnimationUtils.loadAnimation(inflate.getRoot().getContext(), R.anim.button_click);
        List<FamilySystemUpgradeObj> list = this.upgradeAllListFromApp;
        list.clear();
        FamilySystemList familySystemListFromJson = this.mainRoot.getFamilySystemListFromJson();
        if (familySystemListFromJson != null) {
            list.addAll(familySystemListFromJson.getUpgradeList());
        }
        setLogicForClickUpdateItem();
        setUpgradeData();
        setUpgradeDataInView();
        setUpdateDataList();
        FamiliesUpgradesLayoutBinding familiesUpgradesLayoutBinding2 = this.binding;
        if (familiesUpgradesLayoutBinding2 == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            familiesUpgradesLayoutBinding2 = null;
        }
        familiesUpgradesLayoutBinding2.buttonUpgrade.setOnClickListener(new View.OnClickListener() { // from class: com.blackhub.bronline.game.gui.familysystem.UIMenuFamilyUpgrade$$ExternalSyntheticLambda0
            public /* synthetic */ UIMenuFamilyUpgrade$$ExternalSyntheticLambda0() {
            }

            @Override // android.view.View.OnClickListener
            public final void onClick(View view) {
                UIMenuFamilyUpgrade.onCreateView$lambda$5(UIMenuFamilyUpgrade.this, view);
            }
        });
        FamiliesUpgradesLayoutBinding familiesUpgradesLayoutBinding3 = this.binding;
        if (familiesUpgradesLayoutBinding3 == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            familiesUpgradesLayoutBinding3 = null;
        }
        familiesUpgradesLayoutBinding3.buttonBackToMenu.setOnClickListener(new View.OnClickListener() { // from class: com.blackhub.bronline.game.gui.familysystem.UIMenuFamilyUpgrade$$ExternalSyntheticLambda1
            public /* synthetic */ UIMenuFamilyUpgrade$$ExternalSyntheticLambda1() {
            }

            @Override // android.view.View.OnClickListener
            public final void onClick(View view) {
                UIMenuFamilyUpgrade.onCreateView$lambda$6(UIMenuFamilyUpgrade.this, view);
            }
        });
        FamiliesUpgradesLayoutBinding familiesUpgradesLayoutBinding4 = this.binding;
        if (familiesUpgradesLayoutBinding4 == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
        } else {
            familiesUpgradesLayoutBinding = familiesUpgradesLayoutBinding4;
        }
        ConstraintLayout root = familiesUpgradesLayoutBinding.getRoot();
        Intrinsics.checkNotNullExpressionValue(root, "getRoot(...)");
        return root;
    }

    public static final void onCreateView$lambda$5(UIMenuFamilyUpgrade this$0, View view) {
        Intrinsics.checkNotNullParameter(this$0, "this$0");
        Animation animation = this$0.anim;
        if (animation != null) {
            AnyExtensionKt.setActionOnAnimationEnd(animation, new Function0<Unit>() { // from class: com.blackhub.bronline.game.gui.familysystem.UIMenuFamilyUpgrade$onCreateView$2$1
                public UIMenuFamilyUpgrade$onCreateView$2$1() {
                    super(0);
                }

                @Override // kotlin.jvm.functions.Function0
                public /* bridge */ /* synthetic */ Unit invoke() {
                    invoke2();
                    return Unit.INSTANCE;
                }

                /* renamed from: invoke */
                public final void invoke2() {
                    LogicForDialogApply logicForDialogApply;
                    int i;
                    logicForDialogApply = UIMenuFamilyUpgrade.this.logicForDialogApply;
                    if (logicForDialogApply != null) {
                        i = UIMenuFamilyUpgrade.this.thisPrice;
                        logicForDialogApply.showDialog(null, Integer.valueOf(i), null, null);
                    }
                }
            });
        }
        if (view.getAlpha() != 1.0f || this$0.currentUpgradeId == -1) {
            return;
        }
        view.startAnimation(this$0.anim);
    }

    public static final void onCreateView$lambda$6(UIMenuFamilyUpgrade this$0, View view) {
        Intrinsics.checkNotNullParameter(this$0, "this$0");
        ActionsWithJSON actionsWithJSON = this$0.actionsWithJSON;
        if (actionsWithJSON != null) {
            actionsWithJSON.sendGetLayout(5);
        }
    }

    public final void upgradeFamily() {
        ActionsWithJSON actionsWithJSON;
        int i = this.currentUpgradeId;
        if (i == -1 || (actionsWithJSON = this.actionsWithJSON) == null) {
            return;
        }
        actionsWithJSON.setUpgradeForFamily(i);
    }

    public final void setLogicForClickUpdateItem() {
        this.clickUpdateItem = new Function2<FamilyUpgradeData, Integer, Unit>() { // from class: com.blackhub.bronline.game.gui.familysystem.UIMenuFamilyUpgrade$setLogicForClickUpdateItem$1
            public UIMenuFamilyUpgrade$setLogicForClickUpdateItem$1() {
            }

            @Override // kotlin.jvm.functions.Function2
            public /* bridge */ /* synthetic */ Unit invoke(FamilyUpgradeData familyUpgradeData, Integer num) {
                invoke(familyUpgradeData, num.intValue());
                return Unit.INSTANCE;
            }

            public void invoke(@NotNull FamilyUpgradeData updateItem, int position) {
                long j;
                FamilyUpgradeAdapter familyUpgradeAdapter;
                Intrinsics.checkNotNullParameter(updateItem, "updateItem");
                long currentTimeMillis = System.currentTimeMillis();
                j = UIMenuFamilyUpgrade.this.oldTimerForClickOnUpgrade;
                if (currentTimeMillis - j > 1000) {
                    UIMenuFamilyUpgrade.this.oldTimerForClickOnUpgrade = System.currentTimeMillis();
                    updateItem.setClicked(true);
                    familyUpgradeAdapter = UIMenuFamilyUpgrade.this.upgradeAdapter;
                    if (familyUpgradeAdapter != null) {
                        familyUpgradeAdapter.notifyItemChanged(position);
                        familyUpgradeAdapter.setCheckOnlyElement(position);
                    }
                    UIMenuFamilyUpgrade.this.updateTitleForRightBlock(updateItem);
                    UIMenuFamilyUpgrade.this.currentUpgradeId = updateItem.getUpgradeId();
                    UIMenuFamilyUpgrade.this.clearInfoAboutUpgrade(0);
                    UIMenuFamilyUpgrade.this.updateDataForUpgrade(updateItem);
                }
            }
        };
    }

    public final void updateDataForUpgrade(FamilyUpgradeData updateItem) {
        setCurrentUpgradePrice(updateItem.getUpgradeId(), updateItem.getUpgradeCurrentLevel());
        setUpgradeLevels(updateItem.getUpgradeId(), updateItem.getUpgradeCurrentLevel());
        setUpdateDataList();
        setVisibleButtonUpgrade(this.thisPrice);
    }

    public final void updateTitleForRightBlock(FamilyUpgradeData updateItem) {
        FamiliesUpgradesLayoutBinding familiesUpgradesLayoutBinding = this.binding;
        if (familiesUpgradesLayoutBinding == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            familiesUpgradesLayoutBinding = null;
        }
        familiesUpgradesLayoutBinding.upgradeTitle.setText(updateItem.getUpgradeDescription());
    }

    public final void setUpgradeDataInView() {
        this.upgradeAdapter = new FamilyUpgradeAdapter(this.upgradeList, this.clickUpdateItem, this.mainActivity);
        FamiliesUpgradesLayoutBinding familiesUpgradesLayoutBinding = this.binding;
        FamiliesUpgradesLayoutBinding familiesUpgradesLayoutBinding2 = null;
        if (familiesUpgradesLayoutBinding == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            familiesUpgradesLayoutBinding = null;
        }
        RecyclerView recyclerView = familiesUpgradesLayoutBinding.upgradeList;
        FamiliesUpgradesLayoutBinding familiesUpgradesLayoutBinding3 = this.binding;
        if (familiesUpgradesLayoutBinding3 == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
        } else {
            familiesUpgradesLayoutBinding2 = familiesUpgradesLayoutBinding3;
        }
        recyclerView.setLayoutManager(new LinearLayoutManager(familiesUpgradesLayoutBinding2.getRoot().getContext(), 1, false));
        recyclerView.setAdapter(this.upgradeAdapter);
    }

    public final void setUpgradeData() {
        this.maxStoreLevel = 0;
        this.maxWeaponsLevel = 0;
        this.maxFamilyLevel = 0;
        Iterator<FamilySystemUpgradeObj> it = this.upgradeAllListFromApp.iterator();
        while (it.hasNext()) {
            int id = it.next().getId();
            if (id == 0) {
                this.maxStoreLevel++;
            } else if (id == 1) {
                this.maxWeaponsLevel++;
            } else if (id == 2) {
                this.maxFamilyLevel++;
            }
        }
        List<FamilyUpgradeData> list = this.upgradeList;
        list.clear();
        list.add(new FamilyUpgradeData(0, "Улучшить склад", "Склад", 0, this.maxStoreLevel, false, 32, null));
        list.add(new FamilyUpgradeData(1, "Улучшить оружейный склад", "Оружейный склад", 0, this.maxWeaponsLevel, false, 32, null));
        list.add(new FamilyUpgradeData(2, "Улучшить кол-во членов семьи", "Число участников", 0, this.maxFamilyLevel, false, 32, null));
    }

    public final void clearInfoAboutUpgrade(int setVisible) {
        FamiliesUpgradesLayoutBinding familiesUpgradesLayoutBinding = this.binding;
        if (familiesUpgradesLayoutBinding == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            familiesUpgradesLayoutBinding = null;
        }
        familiesUpgradesLayoutBinding.bodyUpgradeInfoNewLayout.setVisibility(setVisible);
        familiesUpgradesLayoutBinding.buttonUpgrade.setVisibility(setVisible);
    }

    public final void setVisibleButtonUpgrade(int getPrice) {
        FamiliesUpgradesLayoutBinding familiesUpgradesLayoutBinding = this.binding;
        if (familiesUpgradesLayoutBinding == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            familiesUpgradesLayoutBinding = null;
        }
        if (getPrice == 0) {
            familiesUpgradesLayoutBinding.buttonUpgrade.setAlpha(0.5f);
        } else {
            familiesUpgradesLayoutBinding.buttonUpgrade.setAlpha(1.0f);
        }
    }

    public final void setCurrentUpgradePrice(int thisId, int thisLevel) {
        if (thisId != 0) {
            if (thisId == 1) {
                if (thisLevel == this.maxWeaponsLevel) {
                    this.thisPrice = 0;
                }
            } else if (thisId == 2 && thisLevel == this.maxFamilyLevel) {
                this.thisPrice = 0;
            }
        } else if (thisLevel == this.maxStoreLevel) {
            this.thisPrice = 0;
        }
        int i = thisLevel + 1;
        for (FamilySystemUpgradeObj familySystemUpgradeObj : this.upgradeAllListFromApp) {
            if (familySystemUpgradeObj.getId() == thisId && familySystemUpgradeObj.getLevel() == i) {
                this.thisPrice = familySystemUpgradeObj.getPrice();
                return;
            }
        }
    }

    public final void setUpgradeLevels(int currentId, int currentLevel) {
        actionWithCollection(this.upgradeCurrentValueList, currentId, currentLevel);
        int i = currentLevel + 1;
        actionWithCollection(this.upgradeNewValueList, currentId, i);
        List<Integer> list = this.levels;
        list.clear();
        FamiliesUpgradesLayoutBinding familiesUpgradesLayoutBinding = null;
        if (this.upgradeNewValueList.size() != 0) {
            list.add(Integer.valueOf(currentLevel));
            list.add(Integer.valueOf(i));
            FamiliesUpgradesLayoutBinding familiesUpgradesLayoutBinding2 = this.binding;
            if (familiesUpgradesLayoutBinding2 == null) {
                Intrinsics.throwUninitializedPropertyAccessException("binding");
            } else {
                familiesUpgradesLayoutBinding = familiesUpgradesLayoutBinding2;
            }
            familiesUpgradesLayoutBinding.viewDotIndicator.setVisibility(0);
            return;
        }
        list.add(Integer.valueOf(currentLevel));
        FamiliesUpgradesLayoutBinding familiesUpgradesLayoutBinding3 = this.binding;
        if (familiesUpgradesLayoutBinding3 == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
        } else {
            familiesUpgradesLayoutBinding = familiesUpgradesLayoutBinding3;
        }
        familiesUpgradesLayoutBinding.viewDotIndicator.setVisibility(8);
    }

    public final List<FamilyUpgradeValueData> actionWithCollection(List<FamilyUpgradeValueData> currentCollection, int currentId, int currentLevel) {
        currentCollection.clear();
        for (FamilySystemUpgradeObj familySystemUpgradeObj : this.upgradeAllListFromApp) {
            if (currentId == familySystemUpgradeObj.getId() && currentLevel == familySystemUpgradeObj.getLevel()) {
                if (familySystemUpgradeObj.getMoney() != 0) {
                    currentCollection.add(setCurrentData("money", familySystemUpgradeObj.getMoney()));
                }
                if (familySystemUpgradeObj.getMaterials() != 0) {
                    currentCollection.add(setCurrentData("materials", familySystemUpgradeObj.getMaterials()));
                }
                if (familySystemUpgradeObj.getMasks() != 0) {
                    currentCollection.add(setCurrentData("masks", familySystemUpgradeObj.getMasks()));
                }
                if (familySystemUpgradeObj.getGasoline() != 0) {
                    currentCollection.add(setCurrentData("gasoline", familySystemUpgradeObj.getGasoline()));
                }
                if (familySystemUpgradeObj.getArmour() != 0) {
                    currentCollection.add(setCurrentData("armour", familySystemUpgradeObj.getArmour()));
                }
                if (familySystemUpgradeObj.getPeople() != 0) {
                    currentCollection.add(setCurrentData("people", familySystemUpgradeObj.getPeople()));
                }
                if (familySystemUpgradeObj.getAmmo() != 0) {
                    currentCollection.add(setCurrentData("ammo", familySystemUpgradeObj.getAmmo()));
                }
            }
        }
        return currentCollection;
    }

    public final FamilyUpgradeValueData setCurrentData(String description, int value) {
        return new FamilyUpgradeValueData(description, value);
    }

    public final void setUpdateDataList() {
        this.familyUpgradeParentItemAdapter = new FamilyUpgradeParentItemAdapter(this.levels, this.upgradeCurrentValueList, this.upgradeNewValueList, this.mainActivity);
        FamiliesUpgradesLayoutBinding familiesUpgradesLayoutBinding = this.binding;
        FamiliesUpgradesLayoutBinding familiesUpgradesLayoutBinding2 = null;
        if (familiesUpgradesLayoutBinding == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            familiesUpgradesLayoutBinding = null;
        }
        RecyclerView recyclerView = familiesUpgradesLayoutBinding.allUpgradeParameters;
        FamiliesUpgradesLayoutBinding familiesUpgradesLayoutBinding3 = this.binding;
        if (familiesUpgradesLayoutBinding3 == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            familiesUpgradesLayoutBinding3 = null;
        }
        recyclerView.setLayoutManager(new LinearLayoutManager(familiesUpgradesLayoutBinding3.getRoot().getContext(), 0, false));
        recyclerView.setAdapter(this.familyUpgradeParentItemAdapter);
        GravitySnapHelper gravitySnapHelper = new GravitySnapHelper(GravityCompat.START);
        FamiliesUpgradesLayoutBinding familiesUpgradesLayoutBinding4 = this.binding;
        if (familiesUpgradesLayoutBinding4 == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            familiesUpgradesLayoutBinding4 = null;
        }
        gravitySnapHelper.attachToRecyclerView(familiesUpgradesLayoutBinding4.allUpgradeParameters);
        FamiliesUpgradesLayoutBinding familiesUpgradesLayoutBinding5 = this.binding;
        if (familiesUpgradesLayoutBinding5 == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
            familiesUpgradesLayoutBinding5 = null;
        }
        DotViewIndicator dotViewIndicator = familiesUpgradesLayoutBinding5.viewDotIndicator;
        dotViewIndicator.setItemCount(2);
        dotViewIndicator.setCurrentPosition(0);
        FamiliesUpgradesLayoutBinding familiesUpgradesLayoutBinding6 = this.binding;
        if (familiesUpgradesLayoutBinding6 == null) {
            Intrinsics.throwUninitializedPropertyAccessException("binding");
        } else {
            familiesUpgradesLayoutBinding2 = familiesUpgradesLayoutBinding6;
        }
        familiesUpgradesLayoutBinding2.allUpgradeParameters.addOnScrollListener(new RecyclerView.OnScrollListener() { // from class: com.blackhub.bronline.game.gui.familysystem.UIMenuFamilyUpgrade$setUpdateDataList$3
            public UIMenuFamilyUpgrade$setUpdateDataList$3() {
            }

            @Override // androidx.recyclerview.widget.RecyclerView.OnScrollListener
            public void onScrollStateChanged(@NotNull RecyclerView recyclerView2, int newState) {
                FamiliesUpgradesLayoutBinding familiesUpgradesLayoutBinding7;
                Intrinsics.checkNotNullParameter(recyclerView2, "recyclerView");
                super.onScrollStateChanged(recyclerView2, newState);
                if (newState == 0) {
                    RecyclerView.LayoutManager layoutManager = recyclerView2.getLayoutManager();
                    Intrinsics.checkNotNull(layoutManager, "null cannot be cast to non-null type androidx.recyclerview.widget.LinearLayoutManager");
                    int findFirstVisibleItemPosition = ((LinearLayoutManager) layoutManager).findFirstVisibleItemPosition();
                    familiesUpgradesLayoutBinding7 = UIMenuFamilyUpgrade.this.binding;
                    if (familiesUpgradesLayoutBinding7 == null) {
                        Intrinsics.throwUninitializedPropertyAccessException("binding");
                        familiesUpgradesLayoutBinding7 = null;
                    }
                    familiesUpgradesLayoutBinding7.viewDotIndicator.setCurrentPosition(findFirstVisibleItemPosition);
                }
            }
        });
    }

    public final void setNullableParameters() {
        this.anim = null;
        this.upgradeAdapter = null;
        this.clickUpdateItem = null;
        this.familyUpgradeParentItemAdapter = null;
        this.upgradeList.clear();
        this.upgradeAllListFromApp.clear();
        this.upgradeCurrentValueList.clear();
        this.upgradeNewValueList.clear();
        this.levels.clear();
    }
}
