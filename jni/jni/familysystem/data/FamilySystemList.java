package com.blackhub.bronline.game.gui.familysystem.data;

import androidx.compose.runtime.internal.StabilityInferred;
import com.google.gson.annotations.SerializedName;
import java.util.ArrayList;
import java.util.List;
import kotlin.Metadata;
import kotlin.jvm.internal.DefaultConstructorMarker;
import kotlin.jvm.internal.Intrinsics;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

/* JADX WARN: Classes with same name are omitted:
  C:\javanew\classes3.dex
 */
/* compiled from: NetworkFamilySystem.kt */
@StabilityInferred(parameters = 0)
@Metadata(d1 = {"\u0000B\n\u0002\u0018\u0002\n\u0002\u0010\u0000\n\u0000\n\u0002\u0010!\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0002\b\u000e\n\u0002\u0010\u000b\n\u0002\b\u0002\n\u0002\u0010\b\n\u0000\n\u0002\u0010\u000e\n\u0000\b\u0087\b\u0018\u00002\u00020\u0001BU\u0012\u000e\b\u0002\u0010\u0002\u001a\b\u0012\u0004\u0012\u00020\u00040\u0003\u0012\u000e\b\u0002\u0010\u0005\u001a\b\u0012\u0004\u0012\u00020\u00060\u0003\u0012\u000e\b\u0002\u0010\u0007\u001a\b\u0012\u0004\u0012\u00020\b0\u0003\u0012\u000e\b\u0002\u0010\t\u001a\b\u0012\u0004\u0012\u00020\n0\u0003\u0012\u000e\b\u0002\u0010\u000b\u001a\b\u0012\u0004\u0012\u00020\f0\u0003¢\u0006\u0002\u0010\rJ\u000f\u0010\u0014\u001a\b\u0012\u0004\u0012\u00020\u00040\u0003HÆ\u0003J\u000f\u0010\u0015\u001a\b\u0012\u0004\u0012\u00020\u00060\u0003HÆ\u0003J\u000f\u0010\u0016\u001a\b\u0012\u0004\u0012\u00020\b0\u0003HÆ\u0003J\u000f\u0010\u0017\u001a\b\u0012\u0004\u0012\u00020\n0\u0003HÆ\u0003J\u000f\u0010\u0018\u001a\b\u0012\u0004\u0012\u00020\f0\u0003HÆ\u0003JY\u0010\u0019\u001a\u00020\u00002\u000e\b\u0002\u0010\u0002\u001a\b\u0012\u0004\u0012\u00020\u00040\u00032\u000e\b\u0002\u0010\u0005\u001a\b\u0012\u0004\u0012\u00020\u00060\u00032\u000e\b\u0002\u0010\u0007\u001a\b\u0012\u0004\u0012\u00020\b0\u00032\u000e\b\u0002\u0010\t\u001a\b\u0012\u0004\u0012\u00020\n0\u00032\u000e\b\u0002\u0010\u000b\u001a\b\u0012\u0004\u0012\u00020\f0\u0003HÆ\u0001J\u0013\u0010\u001a\u001a\u00020\u001b2\b\u0010\u001c\u001a\u0004\u0018\u00010\u0001HÖ\u0003J\t\u0010\u001d\u001a\u00020\u001eHÖ\u0001J\t\u0010\u001f\u001a\u00020 HÖ\u0001R\u001c\u0010\u000b\u001a\b\u0012\u0004\u0012\u00020\f0\u00038\u0006X\u0087\u0004¢\u0006\b\n\u0000\u001a\u0004\b\u000e\u0010\u000fR\u001c\u0010\u0005\u001a\b\u0012\u0004\u0012\u00020\u00060\u00038\u0006X\u0087\u0004¢\u0006\b\n\u0000\u001a\u0004\b\u0010\u0010\u000fR\u001c\u0010\u0007\u001a\b\u0012\u0004\u0012\u00020\b0\u00038\u0006X\u0087\u0004¢\u0006\b\n\u0000\u001a\u0004\b\u0011\u0010\u000fR\u001c\u0010\t\u001a\b\u0012\u0004\u0012\u00020\n0\u00038\u0006X\u0087\u0004¢\u0006\b\n\u0000\u001a\u0004\b\u0012\u0010\u000fR\u001c\u0010\u0002\u001a\b\u0012\u0004\u0012\u00020\u00040\u00038\u0006X\u0087\u0004¢\u0006\b\n\u0000\u001a\u0004\b\u0013\u0010\u000f¨\u0006!"}, d2 = {"Lcom/blackhub/bronline/game/gui/familysystem/data/FamilySystemList;", "", "upgradeList", "", "Lcom/blackhub/bronline/game/gui/familysystem/data/FamilySystemUpgradeObj;", "rewardTopList", "Lcom/blackhub/bronline/game/gui/familysystem/data/FamilySystemRewardTopObj;", "shopList", "Lcom/blackhub/bronline/game/gui/familysystem/data/FamilySystemShopObj;", "tasksList", "Lcom/blackhub/bronline/game/gui/familysystem/data/FamilySystemTasksObj;", "colorsList", "Lcom/blackhub/bronline/game/gui/familysystem/data/FamilySystemColorsObj;", "(Ljava/util/List;Ljava/util/List;Ljava/util/List;Ljava/util/List;Ljava/util/List;)V", "getColorsList", "()Ljava/util/List;", "getRewardTopList", "getShopList", "getTasksList", "getUpgradeList", "component1", "component2", "component3", "component4", "component5", "copy", "equals", "", "other", "hashCode", "", "toString", "", "app_siteRelease"}, k = 1, mv = {1, 9, 0}, xi = 48)
/* loaded from: classes3.dex */
public final /* data */ class FamilySystemList {
    public static final int $stable = 8;

    @SerializedName("Colors")
    @NotNull
    public final List<FamilySystemColorsObj> colorsList;

    @SerializedName("RewardTop")
    @NotNull
    public final List<FamilySystemRewardTopObj> rewardTopList;

    @SerializedName("Shop")
    @NotNull
    public final List<FamilySystemShopObj> shopList;

    @SerializedName("Tasks")
    @NotNull
    public final List<FamilySystemTasksObj> tasksList;

    @SerializedName("Upgrade")
    @NotNull
    public final List<FamilySystemUpgradeObj> upgradeList;

    public FamilySystemList() {
        this(null, null, null, null, null, 31, null);
    }

    public static /* synthetic */ FamilySystemList copy$default(FamilySystemList familySystemList, List list, List list2, List list3, List list4, List list5, int i, Object obj) {
        if ((i & 1) != 0) {
            list = familySystemList.upgradeList;
        }
        if ((i & 2) != 0) {
            list2 = familySystemList.rewardTopList;
        }
        List list6 = list2;
        if ((i & 4) != 0) {
            list3 = familySystemList.shopList;
        }
        List list7 = list3;
        if ((i & 8) != 0) {
            list4 = familySystemList.tasksList;
        }
        List list8 = list4;
        if ((i & 16) != 0) {
            list5 = familySystemList.colorsList;
        }
        return familySystemList.copy(list, list6, list7, list8, list5);
    }

    @NotNull
    public final List<FamilySystemUpgradeObj> component1() {
        return this.upgradeList;
    }

    @NotNull
    public final List<FamilySystemRewardTopObj> component2() {
        return this.rewardTopList;
    }

    @NotNull
    public final List<FamilySystemShopObj> component3() {
        return this.shopList;
    }

    @NotNull
    public final List<FamilySystemTasksObj> component4() {
        return this.tasksList;
    }

    @NotNull
    public final List<FamilySystemColorsObj> component5() {
        return this.colorsList;
    }

    @NotNull
    public final FamilySystemList copy(@NotNull List<FamilySystemUpgradeObj> upgradeList, @NotNull List<FamilySystemRewardTopObj> rewardTopList, @NotNull List<FamilySystemShopObj> shopList, @NotNull List<FamilySystemTasksObj> tasksList, @NotNull List<FamilySystemColorsObj> colorsList) {
        Intrinsics.checkNotNullParameter(upgradeList, "upgradeList");
        Intrinsics.checkNotNullParameter(rewardTopList, "rewardTopList");
        Intrinsics.checkNotNullParameter(shopList, "shopList");
        Intrinsics.checkNotNullParameter(tasksList, "tasksList");
        Intrinsics.checkNotNullParameter(colorsList, "colorsList");
        return new FamilySystemList(upgradeList, rewardTopList, shopList, tasksList, colorsList);
    }

    public boolean equals(@Nullable Object other) {
        if (this == other) {
            return true;
        }
        if (!(other instanceof FamilySystemList)) {
            return false;
        }
        FamilySystemList familySystemList = (FamilySystemList) other;
        return Intrinsics.areEqual(this.upgradeList, familySystemList.upgradeList) && Intrinsics.areEqual(this.rewardTopList, familySystemList.rewardTopList) && Intrinsics.areEqual(this.shopList, familySystemList.shopList) && Intrinsics.areEqual(this.tasksList, familySystemList.tasksList) && Intrinsics.areEqual(this.colorsList, familySystemList.colorsList);
    }

    public int hashCode() {
        return (((((((this.upgradeList.hashCode() * 31) + this.rewardTopList.hashCode()) * 31) + this.shopList.hashCode()) * 31) + this.tasksList.hashCode()) * 31) + this.colorsList.hashCode();
    }

    @NotNull
    public String toString() {
        return "FamilySystemList(upgradeList=" + this.upgradeList + ", rewardTopList=" + this.rewardTopList + ", shopList=" + this.shopList + ", tasksList=" + this.tasksList + ", colorsList=" + this.colorsList + ")";
    }

    public FamilySystemList(@NotNull List<FamilySystemUpgradeObj> upgradeList, @NotNull List<FamilySystemRewardTopObj> rewardTopList, @NotNull List<FamilySystemShopObj> shopList, @NotNull List<FamilySystemTasksObj> tasksList, @NotNull List<FamilySystemColorsObj> colorsList) {
        Intrinsics.checkNotNullParameter(upgradeList, "upgradeList");
        Intrinsics.checkNotNullParameter(rewardTopList, "rewardTopList");
        Intrinsics.checkNotNullParameter(shopList, "shopList");
        Intrinsics.checkNotNullParameter(tasksList, "tasksList");
        Intrinsics.checkNotNullParameter(colorsList, "colorsList");
        this.upgradeList = upgradeList;
        this.rewardTopList = rewardTopList;
        this.shopList = shopList;
        this.tasksList = tasksList;
        this.colorsList = colorsList;
    }

    public /* synthetic */ FamilySystemList(List list, List list2, List list3, List list4, List list5, int i, DefaultConstructorMarker defaultConstructorMarker) {
        this((i & 1) != 0 ? new ArrayList() : list, (i & 2) != 0 ? new ArrayList() : list2, (i & 4) != 0 ? new ArrayList() : list3, (i & 8) != 0 ? new ArrayList() : list4, (i & 16) != 0 ? new ArrayList() : list5);
    }

    @NotNull
    public final List<FamilySystemUpgradeObj> getUpgradeList() {
        return this.upgradeList;
    }

    @NotNull
    public final List<FamilySystemRewardTopObj> getRewardTopList() {
        return this.rewardTopList;
    }

    @NotNull
    public final List<FamilySystemShopObj> getShopList() {
        return this.shopList;
    }

    @NotNull
    public final List<FamilySystemTasksObj> getTasksList() {
        return this.tasksList;
    }

    @NotNull
    public final List<FamilySystemColorsObj> getColorsList() {
        return this.colorsList;
    }
}
