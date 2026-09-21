package com.blackhub.bronline.game.gui.familysystem.adapters;

import android.view.LayoutInflater;
import android.view.ViewGroup;
import android.widget.TextView;
import androidx.compose.runtime.internal.StabilityInferred;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;
import com.blackhub.bronline.R;
import com.blackhub.bronline.databinding.FamiliesUpgradeParentItemBinding;
import com.blackhub.bronline.game.core.JNIActivity;
import com.blackhub.bronline.game.gui.familysystem.data.FamilyUpgradeValueData;
import java.util.List;
import kotlin.Metadata;
import kotlin.jvm.internal.Intrinsics;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

/* JADX WARN: Classes with same name are omitted:
  C:\javanew\classes3.dex
 */
/* compiled from: FamilyUpgradeParentItemAdapter.kt */
@StabilityInferred(parameters = 0)
@Metadata(d1 = {"\u0000@\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0000\n\u0002\u0010 \n\u0002\u0010\b\n\u0000\n\u0002\u0018\u0002\n\u0002\b\u0002\n\u0002\u0018\u0002\n\u0002\b\u0002\n\u0002\u0018\u0002\n\u0002\b\u0002\n\u0002\u0010\u0002\n\u0002\b\u0004\n\u0002\u0018\u0002\n\u0002\b\u0003\b\u0007\u0018\u00002\f\u0012\b\u0012\u00060\u0002R\u00020\u00000\u0001:\u0001\u0017B9\u0012\f\u0010\u0003\u001a\b\u0012\u0004\u0012\u00020\u00050\u0004\u0012\f\u0010\u0006\u001a\b\u0012\u0004\u0012\u00020\u00070\u0004\u0012\f\u0010\b\u001a\b\u0012\u0004\u0012\u00020\u00070\u0004\u0012\b\u0010\t\u001a\u0004\u0018\u00010\n¢\u0006\u0002\u0010\u000bJ\b\u0010\u000e\u001a\u00020\u0005H\u0016J\u001c\u0010\u000f\u001a\u00020\u00102\n\u0010\u0011\u001a\u00060\u0002R\u00020\u00002\u0006\u0010\u0012\u001a\u00020\u0005H\u0016J\u001c\u0010\u0013\u001a\u00060\u0002R\u00020\u00002\u0006\u0010\u0014\u001a\u00020\u00152\u0006\u0010\u0016\u001a\u00020\u0005H\u0016R\u0010\u0010\f\u001a\u0004\u0018\u00010\rX\u0082\u000e¢\u0006\u0002\n\u0000R\u0014\u0010\u0003\u001a\b\u0012\u0004\u0012\u00020\u00050\u0004X\u0082\u0004¢\u0006\u0002\n\u0000R\u0010\u0010\t\u001a\u0004\u0018\u00010\nX\u0082\u0004¢\u0006\u0002\n\u0000R\u0014\u0010\u0006\u001a\b\u0012\u0004\u0012\u00020\u00070\u0004X\u0082\u0004¢\u0006\u0002\n\u0000R\u0014\u0010\b\u001a\b\u0012\u0004\u0012\u00020\u00070\u0004X\u0082\u0004¢\u0006\u0002\n\u0000¨\u0006\u0018"}, d2 = {"Lcom/blackhub/bronline/game/gui/familysystem/adapters/FamilyUpgradeParentItemAdapter;", "Landroidx/recyclerview/widget/RecyclerView$Adapter;", "Lcom/blackhub/bronline/game/gui/familysystem/adapters/FamilyUpgradeParentItemAdapter$ViewHolder;", "levels", "", "", "upgradeCurrentValueList", "Lcom/blackhub/bronline/game/gui/familysystem/data/FamilyUpgradeValueData;", "upgradeNewValueList", "mainActivity", "Lcom/blackhub/bronline/game/core/JNIActivity;", "(Ljava/util/List;Ljava/util/List;Ljava/util/List;Lcom/blackhub/bronline/game/core/JNIActivity;)V", "childAdapter", "Lcom/blackhub/bronline/game/gui/familysystem/adapters/FamilyUpgradeChildItemAdapter;", "getItemCount", "onBindViewHolder", "", "holder", "position", "onCreateViewHolder", "parent", "Landroid/view/ViewGroup;", "viewType", "ViewHolder", "app_siteRelease"}, k = 1, mv = {1, 9, 0}, xi = 48)
/* loaded from: classes3.dex */
public final class FamilyUpgradeParentItemAdapter extends RecyclerView.Adapter<ViewHolder> {
    public static final int $stable = 8;

    @Nullable
    public FamilyUpgradeChildItemAdapter childAdapter;

    @NotNull
    public final List<Integer> levels;

    @Nullable
    public final JNIActivity mainActivity;

    @NotNull
    public final List<FamilyUpgradeValueData> upgradeCurrentValueList;

    @NotNull
    public final List<FamilyUpgradeValueData> upgradeNewValueList;

    public FamilyUpgradeParentItemAdapter(@NotNull List<Integer> levels, @NotNull List<FamilyUpgradeValueData> upgradeCurrentValueList, @NotNull List<FamilyUpgradeValueData> upgradeNewValueList, @Nullable JNIActivity jNIActivity) {
        Intrinsics.checkNotNullParameter(levels, "levels");
        Intrinsics.checkNotNullParameter(upgradeCurrentValueList, "upgradeCurrentValueList");
        Intrinsics.checkNotNullParameter(upgradeNewValueList, "upgradeNewValueList");
        this.levels = levels;
        this.upgradeCurrentValueList = upgradeCurrentValueList;
        this.upgradeNewValueList = upgradeNewValueList;
        this.mainActivity = jNIActivity;
    }

    @Override // androidx.recyclerview.widget.RecyclerView.Adapter
    @NotNull
    public ViewHolder onCreateViewHolder(@NotNull ViewGroup parent, int viewType) {
        Intrinsics.checkNotNullParameter(parent, "parent");
        FamiliesUpgradeParentItemBinding inflate = FamiliesUpgradeParentItemBinding.inflate(LayoutInflater.from(parent.getContext()), parent, false);
        Intrinsics.checkNotNullExpressionValue(inflate, "inflate(...)");
        return new ViewHolder(this, inflate);
    }

    @Override // androidx.recyclerview.widget.RecyclerView.Adapter
    public void onBindViewHolder(@NotNull ViewHolder holder, int position) {
        Intrinsics.checkNotNullParameter(holder, "holder");
        holder.bind(this.levels.get(position).intValue(), position);
    }

    @Override // androidx.recyclerview.widget.RecyclerView.Adapter
    public int getItemCount() {
        return this.levels.size();
    }

    /* JADX WARN: Classes with same name are omitted:
  C:\javanew\classes3.dex
 */
    /* compiled from: FamilyUpgradeParentItemAdapter.kt */
    @Metadata(d1 = {"\u0000 \n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0002\b\u0002\n\u0002\u0010\u0002\n\u0000\n\u0002\u0010\b\n\u0002\b\u0002\b\u0086\u0004\u0018\u00002\u00020\u0001B\r\u0012\u0006\u0010\u0002\u001a\u00020\u0003¢\u0006\u0002\u0010\u0004J\u0016\u0010\u0005\u001a\u00020\u00062\u0006\u0010\u0007\u001a\u00020\b2\u0006\u0010\t\u001a\u00020\bR\u000e\u0010\u0002\u001a\u00020\u0003X\u0082\u0004¢\u0006\u0002\n\u0000¨\u0006\n"}, d2 = {"Lcom/blackhub/bronline/game/gui/familysystem/adapters/FamilyUpgradeParentItemAdapter$ViewHolder;", "Landroidx/recyclerview/widget/RecyclerView$ViewHolder;", "binding", "Lcom/blackhub/bronline/databinding/FamiliesUpgradeParentItemBinding;", "(Lcom/blackhub/bronline/game/gui/familysystem/adapters/FamilyUpgradeParentItemAdapter;Lcom/blackhub/bronline/databinding/FamiliesUpgradeParentItemBinding;)V", "bind", "", "level", "", "position", "app_siteRelease"}, k = 1, mv = {1, 9, 0}, xi = 48)
    public final class ViewHolder extends RecyclerView.ViewHolder {

        @NotNull
        public final FamiliesUpgradeParentItemBinding binding;
        public final /* synthetic */ FamilyUpgradeParentItemAdapter this$0;

        /* JADX WARN: 'super' call moved to the top of the method (can break code semantics) */
        public ViewHolder(@NotNull FamilyUpgradeParentItemAdapter familyUpgradeParentItemAdapter, FamiliesUpgradeParentItemBinding binding) {
            super(binding.getRoot());
            Intrinsics.checkNotNullParameter(binding, "binding");
            this.this$0 = familyUpgradeParentItemAdapter;
            this.binding = binding;
        }

        public final void bind(int level, int position) {
            FamilyUpgradeChildItemAdapter familyUpgradeChildItemAdapter;
            FamiliesUpgradeParentItemBinding familiesUpgradeParentItemBinding = this.binding;
            FamilyUpgradeParentItemAdapter familyUpgradeParentItemAdapter = this.this$0;
            TextView textView = familiesUpgradeParentItemBinding.potentialValue;
            JNIActivity jNIActivity = familyUpgradeParentItemAdapter.mainActivity;
            textView.setText(jNIActivity != null ? jNIActivity.getString(R.string.families_upgrade_level, Integer.valueOf(level)) : null);
            familiesUpgradeParentItemBinding.potentialValue.setSelected(true);
            if (position == 1) {
                familyUpgradeChildItemAdapter = new FamilyUpgradeChildItemAdapter(familyUpgradeParentItemAdapter.upgradeNewValueList, familyUpgradeParentItemAdapter.mainActivity);
            } else {
                familyUpgradeChildItemAdapter = new FamilyUpgradeChildItemAdapter(familyUpgradeParentItemAdapter.upgradeCurrentValueList, familyUpgradeParentItemAdapter.mainActivity);
            }
            familyUpgradeParentItemAdapter.childAdapter = familyUpgradeChildItemAdapter;
            RecyclerView recyclerView = familiesUpgradeParentItemBinding.upgradeValues;
            recyclerView.setLayoutManager(new LinearLayoutManager(familiesUpgradeParentItemBinding.getRoot().getContext(), 1, false));
            recyclerView.setAdapter(familyUpgradeParentItemAdapter.childAdapter);
        }
    }
}
