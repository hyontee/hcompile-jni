package com.blackhub.bronline.game.gui.familysystem.adapters;

import android.view.LayoutInflater;
import android.view.ViewGroup;
import androidx.compose.runtime.internal.StabilityInferred;
import androidx.recyclerview.widget.RecyclerView;
import com.blackhub.bronline.R;
import com.blackhub.bronline.databinding.FamiliesUpgradeChildItemBinding;
import com.blackhub.bronline.game.core.JNIActivity;
import com.blackhub.bronline.game.gui.UsefulKt;
import com.blackhub.bronline.game.gui.familysystem.data.FamilyUpgradeValueData;
import java.util.List;
import kotlin.Metadata;
import kotlin.jvm.internal.Intrinsics;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

/* JADX WARN: Classes with same name are omitted:
  C:\javanew\classes3.dex
 */
/* compiled from: FamilyUpgradeChildItemAdapter.kt */
@StabilityInferred(parameters = 0)
@Metadata(d1 = {"\u00006\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0000\n\u0002\u0010 \n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0002\b\u0002\n\u0002\u0010\b\n\u0000\n\u0002\u0010\u0002\n\u0002\b\u0004\n\u0002\u0018\u0002\n\u0002\b\u0003\b\u0007\u0018\u00002\f\u0012\b\u0012\u00060\u0002R\u00020\u00000\u0001:\u0001\u0013B\u001d\u0012\f\u0010\u0003\u001a\b\u0012\u0004\u0012\u00020\u00050\u0004\u0012\b\u0010\u0006\u001a\u0004\u0018\u00010\u0007¢\u0006\u0002\u0010\bJ\b\u0010\t\u001a\u00020\nH\u0016J\u001c\u0010\u000b\u001a\u00020\f2\n\u0010\r\u001a\u00060\u0002R\u00020\u00002\u0006\u0010\u000e\u001a\u00020\nH\u0016J\u001c\u0010\u000f\u001a\u00060\u0002R\u00020\u00002\u0006\u0010\u0010\u001a\u00020\u00112\u0006\u0010\u0012\u001a\u00020\nH\u0016R\u0010\u0010\u0006\u001a\u0004\u0018\u00010\u0007X\u0082\u0004¢\u0006\u0002\n\u0000R\u0014\u0010\u0003\u001a\b\u0012\u0004\u0012\u00020\u00050\u0004X\u0082\u0004¢\u0006\u0002\n\u0000¨\u0006\u0014"}, d2 = {"Lcom/blackhub/bronline/game/gui/familysystem/adapters/FamilyUpgradeChildItemAdapter;", "Landroidx/recyclerview/widget/RecyclerView$Adapter;", "Lcom/blackhub/bronline/game/gui/familysystem/adapters/FamilyUpgradeChildItemAdapter$ViewHolder;", "upgradeValues", "", "Lcom/blackhub/bronline/game/gui/familysystem/data/FamilyUpgradeValueData;", "mainActivity", "Lcom/blackhub/bronline/game/core/JNIActivity;", "(Ljava/util/List;Lcom/blackhub/bronline/game/core/JNIActivity;)V", "getItemCount", "", "onBindViewHolder", "", "holder", "position", "onCreateViewHolder", "parent", "Landroid/view/ViewGroup;", "viewType", "ViewHolder", "app_siteRelease"}, k = 1, mv = {1, 9, 0}, xi = 48)
/* loaded from: classes3.dex */
public final class FamilyUpgradeChildItemAdapter extends RecyclerView.Adapter<ViewHolder> {
    public static final int $stable = 8;

    @Nullable
    public final JNIActivity mainActivity;

    @NotNull
    public final List<FamilyUpgradeValueData> upgradeValues;

    public FamilyUpgradeChildItemAdapter(@NotNull List<FamilyUpgradeValueData> upgradeValues, @Nullable JNIActivity jNIActivity) {
        Intrinsics.checkNotNullParameter(upgradeValues, "upgradeValues");
        this.upgradeValues = upgradeValues;
        this.mainActivity = jNIActivity;
    }

    @Override // androidx.recyclerview.widget.RecyclerView.Adapter
    @NotNull
    public ViewHolder onCreateViewHolder(@NotNull ViewGroup parent, int viewType) {
        Intrinsics.checkNotNullParameter(parent, "parent");
        FamiliesUpgradeChildItemBinding inflate = FamiliesUpgradeChildItemBinding.inflate(LayoutInflater.from(parent.getContext()), parent, false);
        Intrinsics.checkNotNullExpressionValue(inflate, "inflate(...)");
        return new ViewHolder(this, inflate);
    }

    @Override // androidx.recyclerview.widget.RecyclerView.Adapter
    public void onBindViewHolder(@NotNull ViewHolder holder, int position) {
        Intrinsics.checkNotNullParameter(holder, "holder");
        holder.bind(this.upgradeValues.get(position));
    }

    @Override // androidx.recyclerview.widget.RecyclerView.Adapter
    public int getItemCount() {
        return this.upgradeValues.size();
    }

    /* JADX WARN: Classes with same name are omitted:
  C:\javanew\classes3.dex
 */
    /* compiled from: FamilyUpgradeChildItemAdapter.kt */
    @Metadata(d1 = {"\u0000\u001e\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0002\b\u0002\n\u0002\u0010\u0002\n\u0000\n\u0002\u0018\u0002\n\u0000\b\u0086\u0004\u0018\u00002\u00020\u0001B\r\u0012\u0006\u0010\u0002\u001a\u00020\u0003¢\u0006\u0002\u0010\u0004J\u000e\u0010\u0005\u001a\u00020\u00062\u0006\u0010\u0007\u001a\u00020\bR\u000e\u0010\u0002\u001a\u00020\u0003X\u0082\u0004¢\u0006\u0002\n\u0000¨\u0006\t"}, d2 = {"Lcom/blackhub/bronline/game/gui/familysystem/adapters/FamilyUpgradeChildItemAdapter$ViewHolder;", "Landroidx/recyclerview/widget/RecyclerView$ViewHolder;", "binding", "Lcom/blackhub/bronline/databinding/FamiliesUpgradeChildItemBinding;", "(Lcom/blackhub/bronline/game/gui/familysystem/adapters/FamilyUpgradeChildItemAdapter;Lcom/blackhub/bronline/databinding/FamiliesUpgradeChildItemBinding;)V", "bind", "", "upgradeItem", "Lcom/blackhub/bronline/game/gui/familysystem/data/FamilyUpgradeValueData;", "app_siteRelease"}, k = 1, mv = {1, 9, 0}, xi = 48)
    public final class ViewHolder extends RecyclerView.ViewHolder {

        @NotNull
        public final FamiliesUpgradeChildItemBinding binding;
        public final /* synthetic */ FamilyUpgradeChildItemAdapter this$0;

        /* JADX WARN: 'super' call moved to the top of the method (can break code semantics) */
        public ViewHolder(@NotNull FamilyUpgradeChildItemAdapter familyUpgradeChildItemAdapter, FamiliesUpgradeChildItemBinding binding) {
            super(binding.getRoot());
            Intrinsics.checkNotNullParameter(binding, "binding");
            this.this$0 = familyUpgradeChildItemAdapter;
            this.binding = binding;
        }

        /* JADX WARN: Failed to restore switch over string. Please report as a decompilation issue
        java.lang.NullPointerException: Cannot invoke "java.util.List.iterator()" because the return value of "jadx.core.dex.visitors.regions.SwitchOverStringVisitor$SwitchData.getNewCases()" is null
        	at jadx.core.dex.visitors.regions.SwitchOverStringVisitor.restoreSwitchOverString(SwitchOverStringVisitor.java:109)
        	at jadx.core.dex.visitors.regions.SwitchOverStringVisitor.visitRegion(SwitchOverStringVisitor.java:66)
        	at jadx.core.dex.visitors.regions.DepthRegionTraversal.traverseIterativeStepInternal(DepthRegionTraversal.java:77)
        	at jadx.core.dex.visitors.regions.DepthRegionTraversal.traverseIterativeStepInternal(DepthRegionTraversal.java:82)
         */
        public final void bind(@NotNull FamilyUpgradeValueData upgradeItem) {
            Intrinsics.checkNotNullParameter(upgradeItem, "upgradeItem");
            FamiliesUpgradeChildItemBinding familiesUpgradeChildItemBinding = this.binding;
            JNIActivity jNIActivity = this.this$0.mainActivity;
            if (jNIActivity != null) {
                String description = upgradeItem.getDescription();
                switch (description.hashCode()) {
                    case -1587433046:
                        if (description.equals("gasoline")) {
                            familiesUpgradeChildItemBinding.titleUpgrade.setText("Хранить");
                            familiesUpgradeChildItemBinding.valueUpgrade.setText(jNIActivity.getString(R.string.families_store_oil, Integer.valueOf(upgradeItem.getValue())));
                            break;
                        }
                        break;
                    case -1409300624:
                        if (description.equals("armour")) {
                            familiesUpgradeChildItemBinding.titleUpgrade.setText("Хранить");
                            familiesUpgradeChildItemBinding.valueUpgrade.setText(jNIActivity.getString(R.string.families_store_armor, Integer.valueOf(upgradeItem.getValue())));
                            break;
                        }
                        break;
                    case -991808881:
                        if (description.equals("people")) {
                            familiesUpgradeChildItemBinding.titleUpgrade.setText("Количество членов семьи");
                            familiesUpgradeChildItemBinding.valueUpgrade.setText(jNIActivity.getString(R.string.families_store_people, Integer.valueOf(upgradeItem.getValue())));
                            break;
                        }
                        break;
                    case 2997966:
                        if (description.equals("ammo")) {
                            familiesUpgradeChildItemBinding.titleUpgrade.setText("Хранить");
                            familiesUpgradeChildItemBinding.valueUpgrade.setText(jNIActivity.getString(R.string.families_store_ammo, Integer.valueOf(upgradeItem.getValue())));
                            break;
                        }
                        break;
                    case 103667463:
                        if (description.equals("masks")) {
                            familiesUpgradeChildItemBinding.titleUpgrade.setText("Хранить");
                            familiesUpgradeChildItemBinding.valueUpgrade.setText(jNIActivity.getString(R.string.families_store_masks, Integer.valueOf(upgradeItem.getValue())));
                            break;
                        }
                        break;
                    case 104079552:
                        if (description.equals("money")) {
                            familiesUpgradeChildItemBinding.titleUpgrade.setText("Хранить");
                            familiesUpgradeChildItemBinding.valueUpgrade.setText(jNIActivity.getString(R.string.common_string_with_ruble, UsefulKt.getPriceWithSpaces(Integer.valueOf(upgradeItem.getValue()))));
                            break;
                        }
                        break;
                    case 681132076:
                        if (description.equals("materials")) {
                            familiesUpgradeChildItemBinding.titleUpgrade.setText("Хранить");
                            familiesUpgradeChildItemBinding.valueUpgrade.setText(jNIActivity.getString(R.string.families_store_materials, Integer.valueOf(upgradeItem.getValue())));
                            break;
                        }
                        break;
                }
            }
            familiesUpgradeChildItemBinding.valueUpgrade.setSelected(true);
        }
    }
}
