package com.blackhub.bronline.game.gui.familysystem.data;

import androidx.compose.runtime.internal.StabilityInferred;
import kotlin.Metadata;
import kotlin.jvm.internal.Intrinsics;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

/* JADX WARN: Classes with same name are omitted:
  C:\javanew\classes3.dex
 */
/* compiled from: FamilyPresent.kt */
@StabilityInferred(parameters = 1)
@Metadata(d1 = {"\u0000\"\n\u0002\u0018\u0002\n\u0002\u0010\u0000\n\u0000\n\u0002\u0010\u000e\n\u0002\b\t\n\u0002\u0010\u000b\n\u0002\b\u0002\n\u0002\u0010\b\n\u0002\b\u0002\b\u0087\b\u0018\u00002\u00020\u0001B\u0015\u0012\u0006\u0010\u0002\u001a\u00020\u0003\u0012\u0006\u0010\u0004\u001a\u00020\u0003¢\u0006\u0002\u0010\u0005J\t\u0010\t\u001a\u00020\u0003HÆ\u0003J\t\u0010\n\u001a\u00020\u0003HÆ\u0003J\u001d\u0010\u000b\u001a\u00020\u00002\b\b\u0002\u0010\u0002\u001a\u00020\u00032\b\b\u0002\u0010\u0004\u001a\u00020\u0003HÆ\u0001J\u0013\u0010\f\u001a\u00020\r2\b\u0010\u000e\u001a\u0004\u0018\u00010\u0001HÖ\u0003J\t\u0010\u000f\u001a\u00020\u0010HÖ\u0001J\t\u0010\u0011\u001a\u00020\u0003HÖ\u0001R\u0011\u0010\u0002\u001a\u00020\u0003¢\u0006\b\n\u0000\u001a\u0004\b\u0006\u0010\u0007R\u0011\u0010\u0004\u001a\u00020\u0003¢\u0006\b\n\u0000\u001a\u0004\b\b\u0010\u0007¨\u0006\u0012"}, d2 = {"Lcom/blackhub/bronline/game/gui/familysystem/data/FamilyPresent;", "", "thisPresentsName", "", "thisPresentsValue", "(Ljava/lang/String;Ljava/lang/String;)V", "getThisPresentsName", "()Ljava/lang/String;", "getThisPresentsValue", "component1", "component2", "copy", "equals", "", "other", "hashCode", "", "toString", "app_siteRelease"}, k = 1, mv = {1, 9, 0}, xi = 48)
/* loaded from: classes3.dex */
public final /* data */ class FamilyPresent {
    public static final int $stable = 0;

    @NotNull
    public final String thisPresentsName;

    @NotNull
    public final String thisPresentsValue;

    public static /* synthetic */ FamilyPresent copy$default(FamilyPresent familyPresent, String str, String str2, int i, Object obj) {
        if ((i & 1) != 0) {
            str = familyPresent.thisPresentsName;
        }
        if ((i & 2) != 0) {
            str2 = familyPresent.thisPresentsValue;
        }
        return familyPresent.copy(str, str2);
    }

    @NotNull
    /* renamed from: component1, reason: from getter */
    public final String getThisPresentsName() {
        return this.thisPresentsName;
    }

    @NotNull
    /* renamed from: component2, reason: from getter */
    public final String getThisPresentsValue() {
        return this.thisPresentsValue;
    }

    @NotNull
    public final FamilyPresent copy(@NotNull String thisPresentsName, @NotNull String thisPresentsValue) {
        Intrinsics.checkNotNullParameter(thisPresentsName, "thisPresentsName");
        Intrinsics.checkNotNullParameter(thisPresentsValue, "thisPresentsValue");
        return new FamilyPresent(thisPresentsName, thisPresentsValue);
    }

    public boolean equals(@Nullable Object other) {
        if (this == other) {
            return true;
        }
        if (!(other instanceof FamilyPresent)) {
            return false;
        }
        FamilyPresent familyPresent = (FamilyPresent) other;
        return Intrinsics.areEqual(this.thisPresentsName, familyPresent.thisPresentsName) && Intrinsics.areEqual(this.thisPresentsValue, familyPresent.thisPresentsValue);
    }

    public int hashCode() {
        return (this.thisPresentsName.hashCode() * 31) + this.thisPresentsValue.hashCode();
    }

    @NotNull
    public String toString() {
        return "FamilyPresent(thisPresentsName=" + this.thisPresentsName + ", thisPresentsValue=" + this.thisPresentsValue + ")";
    }

    public FamilyPresent(@NotNull String thisPresentsName, @NotNull String thisPresentsValue) {
        Intrinsics.checkNotNullParameter(thisPresentsName, "thisPresentsName");
        Intrinsics.checkNotNullParameter(thisPresentsValue, "thisPresentsValue");
        this.thisPresentsName = thisPresentsName;
        this.thisPresentsValue = thisPresentsValue;
    }

    @NotNull
    public final String getThisPresentsName() {
        return this.thisPresentsName;
    }

    @NotNull
    public final String getThisPresentsValue() {
        return this.thisPresentsValue;
    }
}
