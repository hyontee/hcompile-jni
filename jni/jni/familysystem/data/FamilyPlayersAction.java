package com.blackhub.bronline.game.gui.familysystem.data;

import androidx.compose.runtime.internal.StabilityInferred;
import kotlin.Metadata;
import kotlin.jvm.internal.Intrinsics;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

/* JADX WARN: Classes with same name are omitted:
  C:\javanew\classes3.dex
 */
/* compiled from: FamilyPlayersAction.kt */
@StabilityInferred(parameters = 0)
@Metadata(d1 = {"\u0000 \n\u0002\u0018\u0002\n\u0002\u0010\u0000\n\u0000\n\u0002\u0010\b\n\u0000\n\u0002\u0010\u000e\n\u0002\b\u0014\n\u0002\u0010\u000b\n\u0002\b\u0004\b\u0087\b\u0018\u00002\u00020\u0001B%\u0012\u0006\u0010\u0002\u001a\u00020\u0003\u0012\u0006\u0010\u0004\u001a\u00020\u0005\u0012\u0006\u0010\u0006\u001a\u00020\u0003\u0012\u0006\u0010\u0007\u001a\u00020\u0003¢\u0006\u0002\u0010\bJ\t\u0010\u0014\u001a\u00020\u0003HÆ\u0003J\t\u0010\u0015\u001a\u00020\u0005HÆ\u0003J\t\u0010\u0016\u001a\u00020\u0003HÆ\u0003J\t\u0010\u0017\u001a\u00020\u0003HÆ\u0003J1\u0010\u0018\u001a\u00020\u00002\b\b\u0002\u0010\u0002\u001a\u00020\u00032\b\b\u0002\u0010\u0004\u001a\u00020\u00052\b\b\u0002\u0010\u0006\u001a\u00020\u00032\b\b\u0002\u0010\u0007\u001a\u00020\u0003HÆ\u0001J\u0013\u0010\u0019\u001a\u00020\u001a2\b\u0010\u001b\u001a\u0004\u0018\u00010\u0001HÖ\u0003J\t\u0010\u001c\u001a\u00020\u0003HÖ\u0001J\t\u0010\u001d\u001a\u00020\u0005HÖ\u0001R\u001a\u0010\u0002\u001a\u00020\u0003X\u0086\u000e¢\u0006\u000e\n\u0000\u001a\u0004\b\t\u0010\n\"\u0004\b\u000b\u0010\fR\u001a\u0010\u0004\u001a\u00020\u0005X\u0086\u000e¢\u0006\u000e\n\u0000\u001a\u0004\b\r\u0010\u000e\"\u0004\b\u000f\u0010\u0010R\u001a\u0010\u0006\u001a\u00020\u0003X\u0086\u000e¢\u0006\u000e\n\u0000\u001a\u0004\b\u0011\u0010\n\"\u0004\b\u0012\u0010\fR\u0011\u0010\u0007\u001a\u00020\u0003¢\u0006\b\n\u0000\u001a\u0004\b\u0013\u0010\n¨\u0006\u001e"}, d2 = {"Lcom/blackhub/bronline/game/gui/familysystem/data/FamilyPlayersAction;", "", "actionsId", "", "actionsTitle", "", "actionsValue", "type", "(ILjava/lang/String;II)V", "getActionsId", "()I", "setActionsId", "(I)V", "getActionsTitle", "()Ljava/lang/String;", "setActionsTitle", "(Ljava/lang/String;)V", "getActionsValue", "setActionsValue", "getType", "component1", "component2", "component3", "component4", "copy", "equals", "", "other", "hashCode", "toString", "app_siteRelease"}, k = 1, mv = {1, 9, 0}, xi = 48)
/* loaded from: classes3.dex */
public final /* data */ class FamilyPlayersAction {
    public static final int $stable = 8;
    public int actionsId;

    @NotNull
    public String actionsTitle;
    public int actionsValue;
    public final int type;

    public static /* synthetic */ FamilyPlayersAction copy$default(FamilyPlayersAction familyPlayersAction, int i, String str, int i2, int i3, int i4, Object obj) {
        if ((i4 & 1) != 0) {
            i = familyPlayersAction.actionsId;
        }
        if ((i4 & 2) != 0) {
            str = familyPlayersAction.actionsTitle;
        }
        if ((i4 & 4) != 0) {
            i2 = familyPlayersAction.actionsValue;
        }
        if ((i4 & 8) != 0) {
            i3 = familyPlayersAction.type;
        }
        return familyPlayersAction.copy(i, str, i2, i3);
    }

    /* renamed from: component1, reason: from getter */
    public final int getActionsId() {
        return this.actionsId;
    }

    @NotNull
    /* renamed from: component2, reason: from getter */
    public final String getActionsTitle() {
        return this.actionsTitle;
    }

    /* renamed from: component3, reason: from getter */
    public final int getActionsValue() {
        return this.actionsValue;
    }

    /* renamed from: component4, reason: from getter */
    public final int getType() {
        return this.type;
    }

    @NotNull
    public final FamilyPlayersAction copy(int actionsId, @NotNull String actionsTitle, int actionsValue, int type) {
        Intrinsics.checkNotNullParameter(actionsTitle, "actionsTitle");
        return new FamilyPlayersAction(actionsId, actionsTitle, actionsValue, type);
    }

    public boolean equals(@Nullable Object other) {
        if (this == other) {
            return true;
        }
        if (!(other instanceof FamilyPlayersAction)) {
            return false;
        }
        FamilyPlayersAction familyPlayersAction = (FamilyPlayersAction) other;
        return this.actionsId == familyPlayersAction.actionsId && Intrinsics.areEqual(this.actionsTitle, familyPlayersAction.actionsTitle) && this.actionsValue == familyPlayersAction.actionsValue && this.type == familyPlayersAction.type;
    }

    public int hashCode() {
        return (((((Integer.hashCode(this.actionsId) * 31) + this.actionsTitle.hashCode()) * 31) + Integer.hashCode(this.actionsValue)) * 31) + Integer.hashCode(this.type);
    }

    @NotNull
    public String toString() {
        return "FamilyPlayersAction(actionsId=" + this.actionsId + ", actionsTitle=" + this.actionsTitle + ", actionsValue=" + this.actionsValue + ", type=" + this.type + ")";
    }

    public FamilyPlayersAction(int i, @NotNull String actionsTitle, int i2, int i3) {
        Intrinsics.checkNotNullParameter(actionsTitle, "actionsTitle");
        this.actionsId = i;
        this.actionsTitle = actionsTitle;
        this.actionsValue = i2;
        this.type = i3;
    }

    public final int getActionsId() {
        return this.actionsId;
    }

    public final void setActionsId(int i) {
        this.actionsId = i;
    }

    @NotNull
    public final String getActionsTitle() {
        return this.actionsTitle;
    }

    public final void setActionsTitle(@NotNull String str) {
        Intrinsics.checkNotNullParameter(str, "<set-?>");
        this.actionsTitle = str;
    }

    public final int getActionsValue() {
        return this.actionsValue;
    }

    public final void setActionsValue(int i) {
        this.actionsValue = i;
    }

    public final int getType() {
        return this.type;
    }
}
