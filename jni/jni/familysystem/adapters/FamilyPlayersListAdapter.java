package com.blackhub.bronline.game.gui.familysystem.adapters;

import android.annotation.SuppressLint;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Filter;
import android.widget.Filterable;
import androidx.compose.runtime.internal.StabilityInferred;
import androidx.core.content.ContextCompat;
import androidx.recyclerview.widget.RecyclerView;
import com.blackhub.bronline.R;
import com.blackhub.bronline.databinding.FamiliesPlayersItemBinding;
import com.blackhub.bronline.game.core.JNIActivity;
import com.blackhub.bronline.game.gui.familysystem.adapters.FamilyPlayersListAdapter;
import com.blackhub.bronline.game.gui.familysystem.data.FamilyPlayer;
import java.util.ArrayList;
import java.util.Collection;
import java.util.List;
import kotlin.Metadata;
import kotlin.Unit;
import kotlin.jvm.functions.Function2;
import kotlin.jvm.internal.Intrinsics;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

/* JADX WARN: Classes with same name are omitted:
  C:\javanew\classes3.dex
 */
/* compiled from: FamilyPlayersListAdapter.kt */
@StabilityInferred(parameters = 0)
@Metadata(d1 = {"\u0000P\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0000\n\u0002\u0010!\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0002\b\u0002\n\u0002\u0010\b\n\u0000\n\u0002\u0010\u0002\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0002\b\u0005\n\u0002\u0018\u0002\n\u0002\b\u0005\n\u0002\u0018\u0002\n\u0002\b\u0005\b\u0007\u0018\u00002\f\u0012\b\u0012\u00060\u0002R\u00020\u00000\u00012\u00020\u0003:\u0001!B]\u0012\f\u0010\u0004\u001a\b\u0012\u0004\u0012\u00020\u00060\u0005\u0012>\u0010\u0007\u001a:\u0012\u0013\u0012\u00110\u0006¢\u0006\f\b\t\u0012\b\b\n\u0012\u0004\b\b(\u000b\u0012\u0013\u0012\u00110\f¢\u0006\f\b\t\u0012\b\b\n\u0012\u0004\b\b(\r\u0012\u0004\u0012\u00020\u000e\u0018\u00010\bj\u0004\u0018\u0001`\u000f\u0012\b\u0010\u0010\u001a\u0004\u0018\u00010\u0011¢\u0006\u0002\u0010\u0012J\b\u0010\u0016\u001a\u00020\u0017H\u0016J\b\u0010\u0018\u001a\u00020\fH\u0016J\u001c\u0010\u0019\u001a\u00020\u000e2\n\u0010\u001a\u001a\u00060\u0002R\u00020\u00002\u0006\u0010\r\u001a\u00020\fH\u0016J\u001c\u0010\u001b\u001a\u00060\u0002R\u00020\u00002\u0006\u0010\u001c\u001a\u00020\u001d2\u0006\u0010\u001e\u001a\u00020\fH\u0016J\u000e\u0010\u001f\u001a\u00020\u000e2\u0006\u0010 \u001a\u00020\fR\u0017\u0010\u0013\u001a\b\u0012\u0004\u0012\u00020\u00060\u0005¢\u0006\b\n\u0000\u001a\u0004\b\u0014\u0010\u0015R\u0010\u0010\u0010\u001a\u0004\u0018\u00010\u0011X\u0082\u0004¢\u0006\u0002\n\u0000RF\u0010\u0007\u001a:\u0012\u0013\u0012\u00110\u0006¢\u0006\f\b\t\u0012\b\b\n\u0012\u0004\b\b(\u000b\u0012\u0013\u0012\u00110\f¢\u0006\f\b\t\u0012\b\b\n\u0012\u0004\b\b(\r\u0012\u0004\u0012\u00020\u000e\u0018\u00010\bj\u0004\u0018\u0001`\u000fX\u0082\u0004¢\u0006\u0002\n\u0000R\u0014\u0010\u0004\u001a\b\u0012\u0004\u0012\u00020\u00060\u0005X\u0082\u0004¢\u0006\u0002\n\u0000¨\u0006\""}, d2 = {"Lcom/blackhub/bronline/game/gui/familysystem/adapters/FamilyPlayersListAdapter;", "Landroidx/recyclerview/widget/RecyclerView$Adapter;", "Lcom/blackhub/bronline/game/gui/familysystem/adapters/FamilyPlayersListAdapter$ViewHolder;", "Landroid/widget/Filterable;", "playersList", "", "Lcom/blackhub/bronline/game/gui/familysystem/data/FamilyPlayer;", "playerClick", "Lkotlin/Function2;", "Lkotlin/ParameterName;", "name", "item", "", "position", "", "Lcom/blackhub/bronline/game/gui/familysystem/OnClickPlayerList;", "mainActivity", "Lcom/blackhub/bronline/game/core/JNIActivity;", "(Ljava/util/List;Lkotlin/jvm/functions/Function2;Lcom/blackhub/bronline/game/core/JNIActivity;)V", "copyList", "getCopyList", "()Ljava/util/List;", "getFilter", "Landroid/widget/Filter;", "getItemCount", "onBindViewHolder", "holder", "onCreateViewHolder", "parent", "Landroid/view/ViewGroup;", "viewType", "setCheckOnlyElement", "checkedPosition", "ViewHolder", "app_siteRelease"}, k = 1, mv = {1, 9, 0}, xi = 48)
/* loaded from: classes3.dex */
public final class FamilyPlayersListAdapter extends RecyclerView.Adapter<ViewHolder> implements Filterable {
    public static final int $stable = 8;

    @NotNull
    public final List<FamilyPlayer> copyList;

    @Nullable
    public final JNIActivity mainActivity;

    @Nullable
    public final Function2<FamilyPlayer, Integer, Unit> playerClick;

    @NotNull
    public final List<FamilyPlayer> playersList;

    /* JADX WARN: Multi-variable type inference failed */
    public FamilyPlayersListAdapter(@NotNull List<FamilyPlayer> playersList, @Nullable Function2<? super FamilyPlayer, ? super Integer, Unit> function2, @Nullable JNIActivity jNIActivity) {
        Intrinsics.checkNotNullParameter(playersList, "playersList");
        this.playersList = playersList;
        this.playerClick = function2;
        this.mainActivity = jNIActivity;
        this.copyList = new ArrayList();
    }

    @NotNull
    public final List<FamilyPlayer> getCopyList() {
        return this.copyList;
    }

    @Override // androidx.recyclerview.widget.RecyclerView.Adapter
    @NotNull
    public ViewHolder onCreateViewHolder(@NotNull ViewGroup parent, int viewType) {
        Intrinsics.checkNotNullParameter(parent, "parent");
        FamiliesPlayersItemBinding inflate = FamiliesPlayersItemBinding.inflate(LayoutInflater.from(parent.getContext()), parent, false);
        Intrinsics.checkNotNullExpressionValue(inflate, "inflate(...)");
        return new ViewHolder(this, inflate);
    }

    @Override // androidx.recyclerview.widget.RecyclerView.Adapter
    public void onBindViewHolder(@NotNull ViewHolder holder, int position) {
        Intrinsics.checkNotNullParameter(holder, "holder");
        holder.bind(this.playersList.get(position), this.playerClick);
    }

    @Override // androidx.recyclerview.widget.RecyclerView.Adapter
    public int getItemCount() {
        return this.playersList.size();
    }

    /* JADX WARN: Classes with same name are omitted:
  C:\javanew\classes3.dex
 */
    /* compiled from: FamilyPlayersListAdapter.kt */
    @Metadata(d1 = {"\u00006\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0002\b\u0002\n\u0002\u0010\u0002\n\u0000\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0002\b\u0002\n\u0002\u0010\b\n\u0000\n\u0002\u0018\u0002\n\u0000\b\u0086\u0004\u0018\u00002\u00020\u0001B\r\u0012\u0006\u0010\u0002\u001a\u00020\u0003¢\u0006\u0002\u0010\u0004JN\u0010\u0005\u001a\u00020\u00062\u0006\u0010\u0007\u001a\u00020\b2>\u0010\t\u001a:\u0012\u0013\u0012\u00110\b¢\u0006\f\b\u000b\u0012\b\b\f\u0012\u0004\b\b(\r\u0012\u0013\u0012\u00110\u000e¢\u0006\f\b\u000b\u0012\b\b\f\u0012\u0004\b\b(\u000f\u0012\u0004\u0012\u00020\u0006\u0018\u00010\nj\u0004\u0018\u0001`\u0010R\u000e\u0010\u0002\u001a\u00020\u0003X\u0082\u0004¢\u0006\u0002\n\u0000¨\u0006\u0011"}, d2 = {"Lcom/blackhub/bronline/game/gui/familysystem/adapters/FamilyPlayersListAdapter$ViewHolder;", "Landroidx/recyclerview/widget/RecyclerView$ViewHolder;", "binding", "Lcom/blackhub/bronline/databinding/FamiliesPlayersItemBinding;", "(Lcom/blackhub/bronline/game/gui/familysystem/adapters/FamilyPlayersListAdapter;Lcom/blackhub/bronline/databinding/FamiliesPlayersItemBinding;)V", "bind", "", "player", "Lcom/blackhub/bronline/game/gui/familysystem/data/FamilyPlayer;", "playerClick", "Lkotlin/Function2;", "Lkotlin/ParameterName;", "name", "item", "", "position", "Lcom/blackhub/bronline/game/gui/familysystem/OnClickPlayerList;", "app_siteRelease"}, k = 1, mv = {1, 9, 0}, xi = 48)
    public final class ViewHolder extends RecyclerView.ViewHolder {

        @NotNull
        public final FamiliesPlayersItemBinding binding;
        public final /* synthetic */ FamilyPlayersListAdapter this$0;

        /* JADX WARN: 'super' call moved to the top of the method (can break code semantics) */
        public ViewHolder(@NotNull FamilyPlayersListAdapter familyPlayersListAdapter, FamiliesPlayersItemBinding binding) {
            super(binding.getRoot());
            Intrinsics.checkNotNullParameter(binding, "binding");
            this.this$0 = familyPlayersListAdapter;
            this.binding = binding;
        }

        public final void bind(@NotNull final FamilyPlayer player, @Nullable final Function2<? super FamilyPlayer, ? super Integer, Unit> playerClick) {
            Intrinsics.checkNotNullParameter(player, "player");
            FamiliesPlayersItemBinding familiesPlayersItemBinding = this.binding;
            FamilyPlayersListAdapter familyPlayersListAdapter = this.this$0;
            familiesPlayersItemBinding.playersRank.setText(String.valueOf(player.getPlayersRank()));
            familiesPlayersItemBinding.playersNick.setText(player.getPlayersNick());
            familiesPlayersItemBinding.playersNick.setSelected(true);
            JNIActivity jNIActivity = familyPlayersListAdapter.mainActivity;
            if (jNIActivity != null) {
                if (player.getPlayersStatus() == 1) {
                    familiesPlayersItemBinding.playersStatus.setBackground(ContextCompat.getDrawable(jNIActivity, R.drawable.bg_rectangle_light_green_cr15));
                } else {
                    familiesPlayersItemBinding.playersStatus.setBackground(ContextCompat.getDrawable(jNIActivity, R.drawable.bg_rectangle_red_cr15));
                }
                if (player.isClicked()) {
                    familiesPlayersItemBinding.getRoot().setBackground(ContextCompat.getDrawable(jNIActivity, R.drawable.bg_rectangle_blue_cr5));
                } else {
                    familiesPlayersItemBinding.getRoot().setBackground(ContextCompat.getDrawable(jNIActivity, R.drawable.bg_rectangle_white15_cr5));
                }
            }
            familiesPlayersItemBinding.getRoot().setOnClickListener(new View.OnClickListener() { // from class: com.blackhub.bronline.game.gui.familysystem.adapters.FamilyPlayersListAdapter$ViewHolder$$ExternalSyntheticLambda0
                @Override // android.view.View.OnClickListener
                public final void onClick(View view) {
                    FamilyPlayersListAdapter.ViewHolder.bind$lambda$2$lambda$1(Function2.this, player, this, view);
                }
            });
        }

        public static final void bind$lambda$2$lambda$1(Function2 function2, FamilyPlayer player, ViewHolder this$0, View view) {
            Intrinsics.checkNotNullParameter(player, "$player");
            Intrinsics.checkNotNullParameter(this$0, "this$0");
            if (function2 != null) {
                function2.invoke(player, Integer.valueOf(this$0.getLayoutPosition()));
            }
        }
    }

    public final void setCheckOnlyElement(int checkedPosition) {
        int size = this.playersList.size();
        for (int i = 0; i < size; i++) {
            if (this.playersList.get(i).isClicked() && i != checkedPosition) {
                this.playersList.get(i).setClicked(false);
                notifyItemChanged(i);
            }
        }
    }

    @Override // android.widget.Filterable
    @NotNull
    public Filter getFilter() {
        return new Filter() { // from class: com.blackhub.bronline.game.gui.familysystem.adapters.FamilyPlayersListAdapter$getFilter$1
            /* JADX WARN: Removed duplicated region for block: B:25:0x0065  */
            @Override // android.widget.Filter
            @org.jetbrains.annotations.NotNull
            /*
                Code decompiled incorrectly, please refer to instructions dump.
                To view partially-correct code enable 'Show inconsistent code' option in preferences
            */
            public android.widget.Filter.FilterResults performFiltering(@org.jetbrains.annotations.Nullable java.lang.CharSequence r11) {
                /*
                    r10 = this;
                    java.util.ArrayList r0 = new java.util.ArrayList
                    r0.<init>()
                    r1 = 0
                    java.lang.String r2 = "toLowerCase(...)"
                    r3 = 0
                    if (r11 == 0) goto L54
                    java.lang.String r11 = r11.toString()
                    if (r11 == 0) goto L54
                    java.util.Locale r4 = java.util.Locale.ROOT
                    java.lang.String r11 = r11.toLowerCase(r4)
                    kotlin.jvm.internal.Intrinsics.checkNotNullExpressionValue(r11, r2)
                    if (r11 == 0) goto L54
                    int r4 = r11.length()
                    r5 = 1
                    int r4 = r4 - r5
                    r6 = r3
                    r7 = r6
                L25:
                    if (r6 > r4) goto L4a
                    if (r7 != 0) goto L2b
                    r8 = r6
                    goto L2c
                L2b:
                    r8 = r4
                L2c:
                    char r8 = r11.charAt(r8)
                    r9 = 32
                    int r8 = kotlin.jvm.internal.Intrinsics.compare(r8, r9)
                    if (r8 > 0) goto L3a
                    r8 = r5
                    goto L3b
                L3a:
                    r8 = r3
                L3b:
                    if (r7 != 0) goto L44
                    if (r8 != 0) goto L41
                    r7 = r5
                    goto L25
                L41:
                    int r6 = r6 + 1
                    goto L25
                L44:
                    if (r8 != 0) goto L47
                    goto L4a
                L47:
                    int r4 = r4 + (-1)
                    goto L25
                L4a:
                    int r4 = r4 + r5
                    java.lang.CharSequence r11 = r11.subSequence(r6, r4)
                    java.lang.String r11 = r11.toString()
                    goto L55
                L54:
                    r11 = r1
                L55:
                    com.blackhub.bronline.game.gui.familysystem.adapters.FamilyPlayersListAdapter r4 = com.blackhub.bronline.game.gui.familysystem.adapters.FamilyPlayersListAdapter.this
                    java.util.List r4 = r4.getCopyList()
                    java.util.Iterator r4 = r4.iterator()
                L5f:
                    boolean r5 = r4.hasNext()
                    if (r5 == 0) goto L8a
                    java.lang.Object r5 = r4.next()
                    com.blackhub.bronline.game.gui.familysystem.data.FamilyPlayer r5 = (com.blackhub.bronline.game.gui.familysystem.data.FamilyPlayer) r5
                    if (r11 == 0) goto L5f
                    java.lang.String r6 = r5.getPlayersNick()
                    java.util.Locale r7 = java.util.Locale.ROOT
                    java.lang.String r8 = "ROOT"
                    kotlin.jvm.internal.Intrinsics.checkNotNullExpressionValue(r7, r8)
                    java.lang.String r6 = r6.toLowerCase(r7)
                    kotlin.jvm.internal.Intrinsics.checkNotNullExpressionValue(r6, r2)
                    r7 = 2
                    boolean r6 = kotlin.text.StringsKt__StringsKt.contains$default(r6, r11, r3, r7, r1)
                    if (r6 == 0) goto L5f
                    r0.add(r5)
                    goto L5f
                L8a:
                    android.widget.Filter$FilterResults r11 = new android.widget.Filter$FilterResults
                    r11.<init>()
                    r11.values = r0
                    return r11
                */
                throw new UnsupportedOperationException("Method not decompiled: com.blackhub.bronline.game.gui.familysystem.adapters.FamilyPlayersListAdapter$getFilter$1.performFiltering(java.lang.CharSequence):android.widget.Filter$FilterResults");
            }

            @Override // android.widget.Filter
            @SuppressLint({"NotifyDataSetChanged"})
            public void publishResults(@Nullable CharSequence p0, @Nullable Filter.FilterResults p1) {
                List list;
                List list2;
                list = FamilyPlayersListAdapter.this.playersList;
                list.clear();
                list2 = FamilyPlayersListAdapter.this.playersList;
                Object obj = p1 != null ? p1.values : null;
                Intrinsics.checkNotNull(obj, "null cannot be cast to non-null type kotlin.collections.Collection<com.blackhub.bronline.game.gui.familysystem.data.FamilyPlayer>");
                list2.addAll((Collection) obj);
                FamilyPlayersListAdapter.this.notifyDataSetChanged();
            }
        };
    }
}
