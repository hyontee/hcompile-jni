package com.blackhub.bronline.game.gui.familysystem.data;

import androidx.compose.runtime.internal.StabilityInferred;
import kotlin.Metadata;
import kotlin.jvm.internal.DefaultConstructorMarker;
import kotlin.jvm.internal.Intrinsics;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

/* JADX WARN: Classes with same name are omitted:
  C:\javanew\classes3.dex
 */
/* compiled from: FamilyUpgradeData.kt */
@StabilityInferred(parameters = 0)
@Metadata(d1 = {"\u0000 \n\u0002\u0018\u0002\n\u0002\u0010\u0000\n\u0000\n\u0002\u0010\b\n\u0000\n\u0002\u0010\u000e\n\u0002\b\u0004\n\u0002\u0010\u000b\n\u0002\b\u001a\b\u0087\b\u0018\u00002\u00020\u0001B7\u0012\u0006\u0010\u0002\u001a\u00020\u0003\u0012\u0006\u0010\u0004\u001a\u00020\u0005\u0012\u0006\u0010\u0006\u001a\u00020\u0005\u0012\u0006\u0010\u0007\u001a\u00020\u0003\u0012\u0006\u0010\b\u001a\u00020\u0003\u0012\b\b\u0002\u0010\t\u001a\u00020\n¢\u0006\u0002\u0010\u000bJ\t\u0010\u0019\u001a\u00020\u0003HÆ\u0003J\t\u0010\u001a\u001a\u00020\u0005HÆ\u0003J\t\u0010\u001b\u001a\u00020\u0005HÆ\u0003J\t\u0010\u001c\u001a\u00020\u0003HÆ\u0003J\t\u0010\u001d\u001a\u00020\u0003HÆ\u0003J\t\u0010\u001e\u001a\u00020\nHÆ\u0003JE\u0010\u001f\u001a\u00020\u00002\b\b\u0002\u0010\u0002\u001a\u00020\u00032\b\b\u0002\u0010\u0004\u001a\u00020\u00052\b\b\u0002\u0010\u0006\u001a\u00020\u00052\b\b\u0002\u0010\u0007\u001a\u00020\u00032\b\b\u0002\u0010\b\u001a\u00020\u00032\b\b\u0002\u0010\t\u001a\u00020\nHÆ\u0001J\u0013\u0010 \u001a\u00020\n2\b\u0010!\u001a\u0004\u0018\u00010\u0001HÖ\u0003J\t\u0010\"\u001a\u00020\u0003HÖ\u0001J\t\u0010#\u001a\u00020\u0005HÖ\u0001R\u001a\u0010\t\u001a\u00020\nX\u0086\u000e¢\u0006\u000e\n\u0000\u001a\u0004\b\t\u0010\f\"\u0004\b\r\u0010\u000eR\u001a\u0010\u0007\u001a\u00020\u0003X\u0086\u000e¢\u0006\u000e\n\u0000\u001a\u0004\b\u000f\u0010\u0010\"\u0004\b\u0011\u0010\u0012R\u0011\u0010\u0006\u001a\u00020\u0005¢\u0006\b\n\u0000\u001a\u0004\b\u0013\u0010\u0014R\u0011\u0010\u0002\u001a\u00020\u0003¢\u0006\b\n\u0000\u001a\u0004\b\u0015\u0010\u0010R\u001a\u0010\b\u001a\u00020\u0003X\u0086\u000e¢\u0006\u000e\n\u0000\u001a\u0004\b\u0016\u0010\u0010\"\u0004\b\u0017\u0010\u0012R\u0011\u0010\u0004\u001a\u00020\u0005¢\u0006\b\n\u0000\u001a\u0004\b\u0018\u0010\u0014¨\u0006$"}, d2 = {"Lcom/blackhub/bronline/game/gui/familysystem/data/FamilyUpgradeData;", "", "upgradeId", "", "upgradeName", "", "upgradeDescription", "upgradeCurrentLevel", "upgradeMaxLevel", "isClicked", "", "(ILjava/lang/String;Ljava/lang/String;IIZ)V", "()Z", "setClicked", "(Z)V", "getUpgradeCurrentLevel", "()I", "setUpgradeCurrentLevel", "(I)V", "getUpgradeDescription", "()Ljava/lang/String;", "getUpgradeId", "getUpgradeMaxLevel", "setUpgradeMaxLevel", "getUpgradeName", "component1", "component2", "component3", "component4", "component5", "component6", "copy", "equals", "other", "hashCode", "toString", "app_siteRelease"}, k = 1, mv = {1, 9, 0}, xi = 48)
/* loaded from: classes3.dex */
public final /* data */ class FamilyUpgradeData {
    public static final int $stable = 8;
    public boolean isClicked;
    public int upgradeCurrentLevel;

    @NotNull
    public final String upgradeDescription;
    public final int upgradeId;
    public int upgradeMaxLevel;

    @NotNull
    public final String upgradeName;

    public static /* synthetic */ FamilyUpgradeData copy$default(FamilyUpgradeData familyUpgradeData, int i, String str, String str2, int i2, int i3, boolean z, int i4, Object obj) {
        if ((i4 & 1) != 0) {
            i = familyUpgradeData.upgradeId;
        }
        if ((i4 & 2) != 0) {
            str = familyUpgradeData.upgradeName;
        }
        String str3 = str;
        if ((i4 & 4) != 0) {
            str2 = familyUpgradeData.upgradeDescription;
        }
        String str4 = str2;
        if ((i4 & 8) != 0) {
            i2 = familyUpgradeData.upgradeCurrentLevel;
        }
        int i5 = i2;
        if ((i4 & 16) != 0) {
            i3 = familyUpgradeData.upgradeMaxLevel;
        }
        int i6 = i3;
        if ((i4 & 32) != 0) {
            z = familyUpgradeData.isClicked;
        }
        return familyUpgradeData.copy(i, str3, str4, i5, i6, z);
    }

    /* renamed from: component1, reason: from getter */
    public final int getUpgradeId() {
        return this.upgradeId;
    }

    @NotNull
    /* renamed from: component2, reason: from getter */
    public final String getUpgradeName() {
        return this.upgradeName;
    }

    @NotNull
    /* renamed from: component3, reason: from getter */
    public final String getUpgradeDescription() {
        return this.upgradeDescription;
    }

    /* renamed from: component4, reason: from getter */
    public final int getUpgradeCurrentLevel() {
        return this.upgradeCurrentLevel;
    }

    /* renamed from: component5, reason: from getter */
    public final int getUpgradeMaxLevel() {
        return this.upgradeMaxLevel;
    }

    /* renamed from: component6, reason: from getter */
    public final boolean getIsClicked() {
        return this.isClicked;
    }

    @NotNull
    public final FamilyUpgradeData copy(int upgradeId, @NotNull String upgradeName, @NotNull String upgradeDescription, int upgradeCurrentLevel, int upgradeMaxLevel, boolean isClicked) {
        Intrinsics.checkNotNullParameter(upgradeName, "upgradeName");
        Intrinsics.checkNotNullParameter(upgradeDescription, "upgradeDescription");
        return new FamilyUpgradeData(upgradeId, upgradeName, upgradeDescription, upgradeCurrentLevel, upgradeMaxLevel, isClicked);
    }

    public boolean equals(@Nullable Object other) {
        if (this == other) {
            return true;
        }
        if (!(other instanceof FamilyUpgradeData)) {
            return false;
        }
        FamilyUpgradeData familyUpgradeData = (FamilyUpgradeData) other;
        return this.upgradeId == familyUpgradeData.upgradeId && Intrinsics.areEqual(this.upgradeName, familyUpgradeData.upgradeName) && Intrinsics.areEqual(this.upgradeDescription, familyUpgradeData.upgradeDescription) && this.upgradeCurrentLevel == familyUpgradeData.upgradeCurrentLevel && this.upgradeMaxLevel == familyUpgradeData.upgradeMaxLevel && this.isClicked == familyUpgradeData.isClicked;
    }

    public int hashCode() {
        return (((((((((Integer.hashCode(this.upgradeId) * 31) + this.upgradeName.hashCode()) * 31) + this.upgradeDescription.hashCode()) * 31) + Integer.hashCode(this.upgradeCurrentLevel)) * 31) + Integer.hashCode(this.upgradeMaxLevel)) * 31) + Boolean.hashCode(this.isClicked);
    }

    @NotNull
    public String toString() {
        return "FamilyUpgradeData(upgradeId=" + this.upgradeId + ", upgradeName=" + this.upgradeName + ", upgradeDescription=" + this.upgradeDescription + ", upgradeCurrentLevel=" + this.upgradeCurrentLevel + ", upgradeMaxLevel=" + this.upgradeMaxLevel + ", isClicked=" + this.isClicked + ")";
    }

    public FamilyUpgradeData(int i, @NotNull String upgradeName, @NotNull String upgradeDescription, int i2, int i3, boolean z) {
        Intrinsics.checkNotNullParameter(upgradeName, "upgradeName");
        Intrinsics.checkNotNullParameter(upgradeDescription, "upgradeDescription");
        this.upgradeId = i;
        this.upgradeName = upgradeName;
        this.upgradeDescription = upgradeDescription;
        this.upgradeCurrentLevel = i2;
        this.upgradeMaxLevel = i3;
        this.isClicked = z;
    }

    public /* synthetic */ FamilyUpgradeData(int i, String str, String str2, int i2, int i3, boolean z, int i4, DefaultConstructorMarker defaultConstructorMarker) {
        this(i, str, str2, i2, i3, (i4 & 32) != 0 ? false : z);
    }

    public final int getUpgradeId() {
        return this.upgradeId;
    }

    @NotNull
    public final String getUpgradeName() {
        return this.upgradeName;
    }

    @NotNull
    public final String getUpgradeDescription() {
        return this.upgradeDescription;
    }

    public final int getUpgradeCurrentLevel() {
        return this.upgradeCurrentLevel;
    }

    public final void setUpgradeCurrentLevel(int i) {
        this.upgradeCurrentLevel = i;
    }

    public final int getUpgradeMaxLevel() {
        return this.upgradeMaxLevel;
    }

    public final void setUpgradeMaxLevel(int i) {
        this.upgradeMaxLevel = i;
    }

    public final boolean isClicked() {
        return this.isClicked;
    }

    public final void setClicked(boolean z) {
        this.isClicked = z;
    }
}
