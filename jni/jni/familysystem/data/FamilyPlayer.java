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
/* compiled from: FamilyPlayer.kt */
@StabilityInferred(parameters = 0)
@Metadata(d1 = {"\u0000 \n\u0002\u0018\u0002\n\u0002\u0010\u0000\n\u0000\n\u0002\u0010\u000e\n\u0000\n\u0002\u0010\b\n\u0002\b\u0002\n\u0002\u0010\u000b\n\u0002\b\u0015\b\u0087\b\u0018\u00002\u00020\u0001B'\u0012\u0006\u0010\u0002\u001a\u00020\u0003\u0012\u0006\u0010\u0004\u001a\u00020\u0005\u0012\u0006\u0010\u0006\u001a\u00020\u0005\u0012\b\b\u0002\u0010\u0007\u001a\u00020\b¢\u0006\u0002\u0010\tJ\t\u0010\u0014\u001a\u00020\u0003HÆ\u0003J\t\u0010\u0015\u001a\u00020\u0005HÆ\u0003J\t\u0010\u0016\u001a\u00020\u0005HÆ\u0003J\t\u0010\u0017\u001a\u00020\bHÆ\u0003J1\u0010\u0018\u001a\u00020\u00002\b\b\u0002\u0010\u0002\u001a\u00020\u00032\b\b\u0002\u0010\u0004\u001a\u00020\u00052\b\b\u0002\u0010\u0006\u001a\u00020\u00052\b\b\u0002\u0010\u0007\u001a\u00020\bHÆ\u0001J\u0013\u0010\u0019\u001a\u00020\b2\b\u0010\u001a\u001a\u0004\u0018\u00010\u0001HÖ\u0003J\t\u0010\u001b\u001a\u00020\u0005HÖ\u0001J\t\u0010\u001c\u001a\u00020\u0003HÖ\u0001R\u001a\u0010\u0007\u001a\u00020\bX\u0086\u000e¢\u0006\u000e\n\u0000\u001a\u0004\b\u0007\u0010\n\"\u0004\b\u000b\u0010\fR\u0011\u0010\u0002\u001a\u00020\u0003¢\u0006\b\n\u0000\u001a\u0004\b\r\u0010\u000eR\u001a\u0010\u0004\u001a\u00020\u0005X\u0086\u000e¢\u0006\u000e\n\u0000\u001a\u0004\b\u000f\u0010\u0010\"\u0004\b\u0011\u0010\u0012R\u0011\u0010\u0006\u001a\u00020\u0005¢\u0006\b\n\u0000\u001a\u0004\b\u0013\u0010\u0010¨\u0006\u001d"}, d2 = {"Lcom/blackhub/bronline/game/gui/familysystem/data/FamilyPlayer;", "", "playersNick", "", "playersRank", "", "playersStatus", "isClicked", "", "(Ljava/lang/String;IIZ)V", "()Z", "setClicked", "(Z)V", "getPlayersNick", "()Ljava/lang/String;", "getPlayersRank", "()I", "setPlayersRank", "(I)V", "getPlayersStatus", "component1", "component2", "component3", "component4", "copy", "equals", "other", "hashCode", "toString", "app_siteRelease"}, k = 1, mv = {1, 9, 0}, xi = 48)
/* loaded from: classes3.dex */
public final /* data */ class FamilyPlayer {
    public static final int $stable = 8;
    public boolean isClicked;

    @NotNull
    public final String playersNick;
    public int playersRank;
    public final int playersStatus;

    public static /* synthetic */ FamilyPlayer copy$default(FamilyPlayer familyPlayer, String str, int i, int i2, boolean z, int i3, Object obj) {
        if ((i3 & 1) != 0) {
            str = familyPlayer.playersNick;
        }
        if ((i3 & 2) != 0) {
            i = familyPlayer.playersRank;
        }
        if ((i3 & 4) != 0) {
            i2 = familyPlayer.playersStatus;
        }
        if ((i3 & 8) != 0) {
            z = familyPlayer.isClicked;
        }
        return familyPlayer.copy(str, i, i2, z);
    }

    @NotNull
    /* renamed from: component1, reason: from getter */
    public final String getPlayersNick() {
        return this.playersNick;
    }

    /* renamed from: component2, reason: from getter */
    public final int getPlayersRank() {
        return this.playersRank;
    }

    /* renamed from: component3, reason: from getter */
    public final int getPlayersStatus() {
        return this.playersStatus;
    }

    /* renamed from: component4, reason: from getter */
    public final boolean getIsClicked() {
        return this.isClicked;
    }

    @NotNull
    public final FamilyPlayer copy(@NotNull String playersNick, int playersRank, int playersStatus, boolean isClicked) {
        Intrinsics.checkNotNullParameter(playersNick, "playersNick");
        return new FamilyPlayer(playersNick, playersRank, playersStatus, isClicked);
    }

    public boolean equals(@Nullable Object other) {
        if (this == other) {
            return true;
        }
        if (!(other instanceof FamilyPlayer)) {
            return false;
        }
        FamilyPlayer familyPlayer = (FamilyPlayer) other;
        return Intrinsics.areEqual(this.playersNick, familyPlayer.playersNick) && this.playersRank == familyPlayer.playersRank && this.playersStatus == familyPlayer.playersStatus && this.isClicked == familyPlayer.isClicked;
    }

    public int hashCode() {
        return (((((this.playersNick.hashCode() * 31) + Integer.hashCode(this.playersRank)) * 31) + Integer.hashCode(this.playersStatus)) * 31) + Boolean.hashCode(this.isClicked);
    }

    @NotNull
    public String toString() {
        return "FamilyPlayer(playersNick=" + this.playersNick + ", playersRank=" + this.playersRank + ", playersStatus=" + this.playersStatus + ", isClicked=" + this.isClicked + ")";
    }

    public FamilyPlayer(@NotNull String playersNick, int i, int i2, boolean z) {
        Intrinsics.checkNotNullParameter(playersNick, "playersNick");
        this.playersNick = playersNick;
        this.playersRank = i;
        this.playersStatus = i2;
        this.isClicked = z;
    }

    public /* synthetic */ FamilyPlayer(String str, int i, int i2, boolean z, int i3, DefaultConstructorMarker defaultConstructorMarker) {
        this(str, i, i2, (i3 & 8) != 0 ? false : z);
    }

    @NotNull
    public final String getPlayersNick() {
        return this.playersNick;
    }

    public final int getPlayersRank() {
        return this.playersRank;
    }

    public final void setPlayersRank(int i) {
        this.playersRank = i;
    }

    public final int getPlayersStatus() {
        return this.playersStatus;
    }

    public final boolean isClicked() {
        return this.isClicked;
    }

    public final void setClicked(boolean z) {
        this.isClicked = z;
    }
}
