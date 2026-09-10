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
/* compiled from: FamilyMainItemData.kt */
@StabilityInferred(parameters = 0)
@Metadata(d1 = {"\u0000\u001e\n\u0002\u0018\u0002\n\u0002\u0010\u0000\n\u0000\n\u0002\u0010\b\n\u0000\n\u0002\u0010\u000e\n\u0000\n\u0002\u0010\u000b\n\u0002\b\u0011\b\u0087\b\u0018\u00002\u00020\u0001B\u001f\u0012\u0006\u0010\u0002\u001a\u00020\u0003\u0012\u0006\u0010\u0004\u001a\u00020\u0005\u0012\b\b\u0002\u0010\u0006\u001a\u00020\u0007¢\u0006\u0002\u0010\bJ\t\u0010\u0010\u001a\u00020\u0003HÆ\u0003J\t\u0010\u0011\u001a\u00020\u0005HÆ\u0003J\t\u0010\u0012\u001a\u00020\u0007HÆ\u0003J'\u0010\u0013\u001a\u00020\u00002\b\b\u0002\u0010\u0002\u001a\u00020\u00032\b\b\u0002\u0010\u0004\u001a\u00020\u00052\b\b\u0002\u0010\u0006\u001a\u00020\u0007HÆ\u0001J\u0013\u0010\u0014\u001a\u00020\u00072\b\u0010\u0015\u001a\u0004\u0018\u00010\u0001HÖ\u0003J\t\u0010\u0016\u001a\u00020\u0003HÖ\u0001J\t\u0010\u0017\u001a\u00020\u0005HÖ\u0001R\u001a\u0010\u0006\u001a\u00020\u0007X\u0086\u000e¢\u0006\u000e\n\u0000\u001a\u0004\b\u0006\u0010\t\"\u0004\b\n\u0010\u000bR\u0011\u0010\u0002\u001a\u00020\u0003¢\u0006\b\n\u0000\u001a\u0004\b\f\u0010\rR\u0011\u0010\u0004\u001a\u00020\u0005¢\u0006\b\n\u0000\u001a\u0004\b\u000e\u0010\u000f¨\u0006\u0018"}, d2 = {"Lcom/blackhub/bronline/game/gui/familysystem/data/FamilyMainItemData;", "", "itemsId", "", "itemsTitle", "", "isClicked", "", "(ILjava/lang/String;Z)V", "()Z", "setClicked", "(Z)V", "getItemsId", "()I", "getItemsTitle", "()Ljava/lang/String;", "component1", "component2", "component3", "copy", "equals", "other", "hashCode", "toString", "app_siteRelease"}, k = 1, mv = {1, 9, 0}, xi = 48)
/* loaded from: classes3.dex */
public final /* data */ class FamilyMainItemData {
    public static final int $stable = 8;
    public boolean isClicked;
    public final int itemsId;

    @NotNull
    public final String itemsTitle;

    public static /* synthetic */ FamilyMainItemData copy$default(FamilyMainItemData familyMainItemData, int i, String str, boolean z, int i2, Object obj) {
        if ((i2 & 1) != 0) {
            i = familyMainItemData.itemsId;
        }
        if ((i2 & 2) != 0) {
            str = familyMainItemData.itemsTitle;
        }
        if ((i2 & 4) != 0) {
            z = familyMainItemData.isClicked;
        }
        return familyMainItemData.copy(i, str, z);
    }

    /* renamed from: component1, reason: from getter */
    public final int getItemsId() {
        return this.itemsId;
    }

    @NotNull
    /* renamed from: component2, reason: from getter */
    public final String getItemsTitle() {
        return this.itemsTitle;
    }

    /* renamed from: component3, reason: from getter */
    public final boolean getIsClicked() {
        return this.isClicked;
    }

    @NotNull
    public final FamilyMainItemData copy(int itemsId, @NotNull String itemsTitle, boolean isClicked) {
        Intrinsics.checkNotNullParameter(itemsTitle, "itemsTitle");
        return new FamilyMainItemData(itemsId, itemsTitle, isClicked);
    }

    public boolean equals(@Nullable Object other) {
        if (this == other) {
            return true;
        }
        if (!(other instanceof FamilyMainItemData)) {
            return false;
        }
        FamilyMainItemData familyMainItemData = (FamilyMainItemData) other;
        return this.itemsId == familyMainItemData.itemsId && Intrinsics.areEqual(this.itemsTitle, familyMainItemData.itemsTitle) && this.isClicked == familyMainItemData.isClicked;
    }

    public int hashCode() {
        return (((Integer.hashCode(this.itemsId) * 31) + this.itemsTitle.hashCode()) * 31) + Boolean.hashCode(this.isClicked);
    }

    @NotNull
    public String toString() {
        return "FamilyMainItemData(itemsId=" + this.itemsId + ", itemsTitle=" + this.itemsTitle + ", isClicked=" + this.isClicked + ")";
    }

    public FamilyMainItemData(int i, @NotNull String itemsTitle, boolean z) {
        Intrinsics.checkNotNullParameter(itemsTitle, "itemsTitle");
        this.itemsId = i;
        this.itemsTitle = itemsTitle;
        this.isClicked = z;
    }

    public /* synthetic */ FamilyMainItemData(int i, String str, boolean z, int i2, DefaultConstructorMarker defaultConstructorMarker) {
        this(i, str, (i2 & 4) != 0 ? false : z);
    }

    public final int getItemsId() {
        return this.itemsId;
    }

    @NotNull
    public final String getItemsTitle() {
        return this.itemsTitle;
    }

    public final boolean isClicked() {
        return this.isClicked;
    }

    public final void setClicked(boolean z) {
        this.isClicked = z;
    }
}
