package com.blackhub.bronline.game.gui.familysystem.data;

import androidx.compose.runtime.internal.StabilityInferred;
import kotlin.Metadata;
import kotlin.jvm.internal.Intrinsics;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

/* JADX WARN: Classes with same name are omitted:
  C:\javanew\classes3.dex
 */
/* compiled from: FamilyUpgradeValueData.kt */
@StabilityInferred(parameters = 1)
@Metadata(d1 = {"\u0000 \n\u0002\u0018\u0002\n\u0002\u0010\u0000\n\u0000\n\u0002\u0010\u000e\n\u0000\n\u0002\u0010\b\n\u0002\b\t\n\u0002\u0010\u000b\n\u0002\b\u0004\b\u0087\b\u0018\u00002\u00020\u0001B\u0015\u0012\u0006\u0010\u0002\u001a\u00020\u0003\u0012\u0006\u0010\u0004\u001a\u00020\u0005¢\u0006\u0002\u0010\u0006J\t\u0010\u000b\u001a\u00020\u0003HÆ\u0003J\t\u0010\f\u001a\u00020\u0005HÆ\u0003J\u001d\u0010\r\u001a\u00020\u00002\b\b\u0002\u0010\u0002\u001a\u00020\u00032\b\b\u0002\u0010\u0004\u001a\u00020\u0005HÆ\u0001J\u0013\u0010\u000e\u001a\u00020\u000f2\b\u0010\u0010\u001a\u0004\u0018\u00010\u0001HÖ\u0003J\t\u0010\u0011\u001a\u00020\u0005HÖ\u0001J\t\u0010\u0012\u001a\u00020\u0003HÖ\u0001R\u0011\u0010\u0002\u001a\u00020\u0003¢\u0006\b\n\u0000\u001a\u0004\b\u0007\u0010\bR\u0011\u0010\u0004\u001a\u00020\u0005¢\u0006\b\n\u0000\u001a\u0004\b\t\u0010\n¨\u0006\u0013"}, d2 = {"Lcom/blackhub/bronline/game/gui/familysystem/data/FamilyUpgradeValueData;", "", "description", "", "value", "", "(Ljava/lang/String;I)V", "getDescription", "()Ljava/lang/String;", "getValue", "()I", "component1", "component2", "copy", "equals", "", "other", "hashCode", "toString", "app_siteRelease"}, k = 1, mv = {1, 9, 0}, xi = 48)
/* loaded from: classes3.dex */
public final /* data */ class FamilyUpgradeValueData {
    public static final int $stable = 0;

    @NotNull
    public final String description;
    public final int value;

    public static /* synthetic */ FamilyUpgradeValueData copy$default(FamilyUpgradeValueData familyUpgradeValueData, String str, int i, int i2, Object obj) {
        if ((i2 & 1) != 0) {
            str = familyUpgradeValueData.description;
        }
        if ((i2 & 2) != 0) {
            i = familyUpgradeValueData.value;
        }
        return familyUpgradeValueData.copy(str, i);
    }

    @NotNull
    /* renamed from: component1, reason: from getter */
    public final String getDescription() {
        return this.description;
    }

    /* renamed from: component2, reason: from getter */
    public final int getValue() {
        return this.value;
    }

    @NotNull
    public final FamilyUpgradeValueData copy(@NotNull String description, int value) {
        Intrinsics.checkNotNullParameter(description, "description");
        return new FamilyUpgradeValueData(description, value);
    }

    public boolean equals(@Nullable Object other) {
        if (this == other) {
            return true;
        }
        if (!(other instanceof FamilyUpgradeValueData)) {
            return false;
        }
        FamilyUpgradeValueData familyUpgradeValueData = (FamilyUpgradeValueData) other;
        return Intrinsics.areEqual(this.description, familyUpgradeValueData.description) && this.value == familyUpgradeValueData.value;
    }

    public int hashCode() {
        return (this.description.hashCode() * 31) + Integer.hashCode(this.value);
    }

    @NotNull
    public String toString() {
        return "FamilyUpgradeValueData(description=" + this.description + ", value=" + this.value + ")";
    }

    public FamilyUpgradeValueData(@NotNull String description, int i) {
        Intrinsics.checkNotNullParameter(description, "description");
        this.description = description;
        this.value = i;
    }

    @NotNull
    public final String getDescription() {
        return this.description;
    }

    public final int getValue() {
        return this.value;
    }
}
