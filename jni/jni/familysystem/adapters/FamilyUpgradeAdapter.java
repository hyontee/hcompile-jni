package com.blackhub.bronline.game.gui.familysystem.adapters;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import androidx.compose.runtime.internal.StabilityInferred;
import androidx.core.content.ContextCompat;
import androidx.recyclerview.widget.RecyclerView;
import com.blackhub.bronline.R;
import com.blackhub.bronline.databinding.FamiliesMenuUpgradeItemBinding;
import com.blackhub.bronline.game.core.JNIActivity;
import com.blackhub.bronline.game.gui.familysystem.adapters.FamilyUpgradeAdapter;
import com.blackhub.bronline.game.gui.familysystem.data.FamilyUpgradeData;
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
/* compiled from: FamilyUpgradeAdapter.kt */
@StabilityInferred(parameters = 0)
@Metadata(d1 = {"\u0000D\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0000\n\u0002\u0010!\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0002\b\u0002\n\u0002\u0010\b\n\u0000\n\u0002\u0010\u0002\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0002\b\u0006\n\u0002\u0018\u0002\n\u0002\b\u0005\b\u0007\u0018\u00002\f\u0012\b\u0012\u00060\u0002R\u00020\u00000\u0001:\u0001\u001bB]\u0012\f\u0010\u0003\u001a\b\u0012\u0004\u0012\u00020\u00050\u0004\u0012>\u0010\u0006\u001a:\u0012\u0013\u0012\u00110\u0005¢\u0006\f\b\b\u0012\b\b\t\u0012\u0004\b\b(\n\u0012\u0013\u0012\u00110\u000b¢\u0006\f\b\b\u0012\b\b\t\u0012\u0004\b\b(\f\u0012\u0004\u0012\u00020\r\u0018\u00010\u0007j\u0004\u0018\u0001`\u000e\u0012\b\u0010\u000f\u001a\u0004\u0018\u00010\u0010¢\u0006\u0002\u0010\u0011J\b\u0010\u0012\u001a\u00020\u000bH\u0016J\u001c\u0010\u0013\u001a\u00020\r2\n\u0010\u0014\u001a\u00060\u0002R\u00020\u00002\u0006\u0010\f\u001a\u00020\u000bH\u0016J\u001c\u0010\u0015\u001a\u00060\u0002R\u00020\u00002\u0006\u0010\u0016\u001a\u00020\u00172\u0006\u0010\u0018\u001a\u00020\u000bH\u0016J\u000e\u0010\u0019\u001a\u00020\r2\u0006\u0010\u001a\u001a\u00020\u000bRF\u0010\u0006\u001a:\u0012\u0013\u0012\u00110\u0005¢\u0006\f\b\b\u0012\b\b\t\u0012\u0004\b\b(\n\u0012\u0013\u0012\u00110\u000b¢\u0006\f\b\b\u0012\b\b\t\u0012\u0004\b\b(\f\u0012\u0004\u0012\u00020\r\u0018\u00010\u0007j\u0004\u0018\u0001`\u000eX\u0082\u0004¢\u0006\u0002\n\u0000R\u0010\u0010\u000f\u001a\u0004\u0018\u00010\u0010X\u0082\u0004¢\u0006\u0002\n\u0000R\u0014\u0010\u0003\u001a\b\u0012\u0004\u0012\u00020\u00050\u0004X\u0082\u0004¢\u0006\u0002\n\u0000¨\u0006\u001c"}, d2 = {"Lcom/blackhub/bronline/game/gui/familysystem/adapters/FamilyUpgradeAdapter;", "Landroidx/recyclerview/widget/RecyclerView$Adapter;", "Lcom/blackhub/bronline/game/gui/familysystem/adapters/FamilyUpgradeAdapter$ViewHolder;", "upgradeList", "", "Lcom/blackhub/bronline/game/gui/familysystem/data/FamilyUpgradeData;", "clickUpdateItem", "Lkotlin/Function2;", "Lkotlin/ParameterName;", "name", "updateItem", "", "position", "", "Lcom/blackhub/bronline/game/gui/familysystem/OnClickUpdateItem;", "mainActivity", "Lcom/blackhub/bronline/game/core/JNIActivity;", "(Ljava/util/List;Lkotlin/jvm/functions/Function2;Lcom/blackhub/bronline/game/core/JNIActivity;)V", "getItemCount", "onBindViewHolder", "holder", "onCreateViewHolder", "parent", "Landroid/view/ViewGroup;", "viewType", "setCheckOnlyElement", "checkedPosition", "ViewHolder", "app_siteRelease"}, k = 1, mv = {1, 9, 0}, xi = 48)
/* loaded from: classes3.dex */
public final class FamilyUpgradeAdapter extends RecyclerView.Adapter<ViewHolder> {
    public static final int $stable = 8;

    @Nullable
    public final Function2<FamilyUpgradeData, Integer, Unit> clickUpdateItem;

    @Nullable
    public final JNIActivity mainActivity;

    @NotNull
    public final List<FamilyUpgradeData> upgradeList;

    /* JADX WARN: Multi-variable type inference failed */
    public FamilyUpgradeAdapter(@NotNull List<FamilyUpgradeData> upgradeList, @Nullable Function2<? super FamilyUpgradeData, ? super Integer, Unit> function2, @Nullable JNIActivity jNIActivity) {
        Intrinsics.checkNotNullParameter(upgradeList, "upgradeList");
        this.upgradeList = upgradeList;
        this.clickUpdateItem = function2;
        this.mainActivity = jNIActivity;
    }

    @Override // androidx.recyclerview.widget.RecyclerView.Adapter
    @NotNull
    public ViewHolder onCreateViewHolder(@NotNull ViewGroup parent, int viewType) {
        Intrinsics.checkNotNullParameter(parent, "parent");
        FamiliesMenuUpgradeItemBinding inflate = FamiliesMenuUpgradeItemBinding.inflate(LayoutInflater.from(parent.getContext()), parent, false);
        Intrinsics.checkNotNullExpressionValue(inflate, "inflate(...)");
        return new ViewHolder(this, inflate);
    }

    @Override // androidx.recyclerview.widget.RecyclerView.Adapter
    public void onBindViewHolder(@NotNull ViewHolder holder, int position) {
        Intrinsics.checkNotNullParameter(holder, "holder");
        holder.bind(this.upgradeList.get(position), this.clickUpdateItem);
    }

    @Override // androidx.recyclerview.widget.RecyclerView.Adapter
    public int getItemCount() {
        return this.upgradeList.size();
    }

    /* JADX WARN: Classes with same name are omitted:
  C:\javanew\classes3.dex
 */
    /* compiled from: FamilyUpgradeAdapter.kt */
    @Metadata(d1 = {"\u00006\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0002\b\u0002\n\u0002\u0010\u0002\n\u0000\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0002\b\u0002\n\u0002\u0010\b\n\u0000\n\u0002\u0018\u0002\n\u0000\b\u0086\u0004\u0018\u00002\u00020\u0001B\r\u0012\u0006\u0010\u0002\u001a\u00020\u0003¢\u0006\u0002\u0010\u0004JN\u0010\u0005\u001a\u00020\u00062\u0006\u0010\u0007\u001a\u00020\b2>\u0010\t\u001a:\u0012\u0013\u0012\u00110\b¢\u0006\f\b\u000b\u0012\b\b\f\u0012\u0004\b\b(\r\u0012\u0013\u0012\u00110\u000e¢\u0006\f\b\u000b\u0012\b\b\f\u0012\u0004\b\b(\u000f\u0012\u0004\u0012\u00020\u0006\u0018\u00010\nj\u0004\u0018\u0001`\u0010R\u000e\u0010\u0002\u001a\u00020\u0003X\u0082\u0004¢\u0006\u0002\n\u0000¨\u0006\u0011"}, d2 = {"Lcom/blackhub/bronline/game/gui/familysystem/adapters/FamilyUpgradeAdapter$ViewHolder;", "Landroidx/recyclerview/widget/RecyclerView$ViewHolder;", "binding", "Lcom/blackhub/bronline/databinding/FamiliesMenuUpgradeItemBinding;", "(Lcom/blackhub/bronline/game/gui/familysystem/adapters/FamilyUpgradeAdapter;Lcom/blackhub/bronline/databinding/FamiliesMenuUpgradeItemBinding;)V", "bind", "", "upgradeItem", "Lcom/blackhub/bronline/game/gui/familysystem/data/FamilyUpgradeData;", "clickUpdateItem", "Lkotlin/Function2;", "Lkotlin/ParameterName;", "name", "updateItem", "", "position", "Lcom/blackhub/bronline/game/gui/familysystem/OnClickUpdateItem;", "app_siteRelease"}, k = 1, mv = {1, 9, 0}, xi = 48)
    public final class ViewHolder extends RecyclerView.ViewHolder {

        @NotNull
        public final FamiliesMenuUpgradeItemBinding binding;
        public final /* synthetic */ FamilyUpgradeAdapter this$0;

        /* JADX WARN: 'super' call moved to the top of the method (can break code semantics) */
        public ViewHolder(@NotNull FamilyUpgradeAdapter familyUpgradeAdapter, FamiliesMenuUpgradeItemBinding binding) {
            super(binding.getRoot());
            Intrinsics.checkNotNullParameter(binding, "binding");
            this.this$0 = familyUpgradeAdapter;
            this.binding = binding;
        }

        public final void bind(@NotNull final FamilyUpgradeData upgradeItem, @Nullable final Function2<? super FamilyUpgradeData, ? super Integer, Unit> clickUpdateItem) {
            Intrinsics.checkNotNullParameter(upgradeItem, "upgradeItem");
            FamiliesMenuUpgradeItemBinding familiesMenuUpgradeItemBinding = this.binding;
            FamilyUpgradeAdapter familyUpgradeAdapter = this.this$0;
            familiesMenuUpgradeItemBinding.upgradeTitle.setText(upgradeItem.getUpgradeName());
            JNIActivity jNIActivity = familyUpgradeAdapter.mainActivity;
            if (jNIActivity != null) {
                familiesMenuUpgradeItemBinding.upgradeValue.setText(jNIActivity.getString(R.string.families_upgrade_value, Integer.valueOf(upgradeItem.getUpgradeCurrentLevel()), Integer.valueOf(upgradeItem.getUpgradeMaxLevel())));
                if (upgradeItem.isClicked()) {
                    familiesMenuUpgradeItemBinding.getRoot().setBackground(ContextCompat.getDrawable(jNIActivity, R.drawable.bg_rectangle_blue_cr5));
                } else {
                    familiesMenuUpgradeItemBinding.getRoot().setBackground(ContextCompat.getDrawable(jNIActivity, R.drawable.bg_rectangle_white15_cr5));
                }
            }
            familiesMenuUpgradeItemBinding.getRoot().setOnClickListener(new View.OnClickListener() { // from class: com.blackhub.bronline.game.gui.familysystem.adapters.FamilyUpgradeAdapter$ViewHolder$$ExternalSyntheticLambda0
                @Override // android.view.View.OnClickListener
                public final void onClick(View view) {
                    FamilyUpgradeAdapter.ViewHolder.bind$lambda$2$lambda$1(Function2.this, upgradeItem, this, view);
                }
            });
        }

        public static final void bind$lambda$2$lambda$1(Function2 function2, FamilyUpgradeData upgradeItem, ViewHolder this$0, View view) {
            Intrinsics.checkNotNullParameter(upgradeItem, "$upgradeItem");
            Intrinsics.checkNotNullParameter(this$0, "this$0");
            if (function2 != null) {
                function2.invoke(upgradeItem, Integer.valueOf(this$0.getLayoutPosition()));
            }
        }
    }

    public final void setCheckOnlyElement(int checkedPosition) {
        int size = this.upgradeList.size();
        for (int i = 0; i < size; i++) {
            if (this.upgradeList.get(i).isClicked() && i != checkedPosition) {
                this.upgradeList.get(i).setClicked(false);
                notifyItemChanged(i);
            }
        }
    }
}
