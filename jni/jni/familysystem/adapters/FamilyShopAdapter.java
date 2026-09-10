package com.blackhub.bronline.game.gui.familysystem.adapters;

import android.graphics.Bitmap;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import androidx.collection.ArrayMap;
import androidx.compose.runtime.internal.StabilityInferred;
import androidx.core.content.res.ResourcesCompat;
import androidx.recyclerview.widget.RecyclerView;
import com.blackhub.bronline.R;
import com.blackhub.bronline.databinding.FamiliesShopItemBinding;
import com.blackhub.bronline.game.gui.UsefulKt;
import com.blackhub.bronline.game.gui.familysystem.FamilyUtilsKt;
import com.blackhub.bronline.game.gui.familysystem.adapters.FamilyShopAdapter;
import com.blackhub.bronline.game.gui.familysystem.data.FamilySystemShopObj;
import java.util.List;
import kotlin.Metadata;
import kotlin.Unit;
import kotlin.jvm.functions.Function2;
import kotlin.jvm.internal.Intrinsics;
import kotlin.jvm.internal.SourceDebugExtension;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

/* JADX WARN: Classes with same name are omitted:
  C:\javanew\classes3.dex
 */
/* compiled from: FamilyShopAdapter.kt */
@StabilityInferred(parameters = 0)
@Metadata(d1 = {"\u0000J\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0000\n\u0002\u0010!\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0002\b\u0002\n\u0002\u0010\b\n\u0000\n\u0002\u0010\u0002\n\u0002\u0018\u0002\n\u0002\b\u0002\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0002\b\u0005\n\u0002\u0018\u0002\n\u0002\b\u0005\b\u0007\u0018\u00002\f\u0012\b\u0012\u00060\u0002R\u00020\u00000\u0001:\u0001\u001cBS\u0012\f\u0010\u0003\u001a\b\u0012\u0004\u0012\u00020\u00050\u0004\u0012>\u0010\u0006\u001a:\u0012\u0013\u0012\u00110\u0005¢\u0006\f\b\b\u0012\b\b\t\u0012\u0004\b\b(\n\u0012\u0013\u0012\u00110\u000b¢\u0006\f\b\b\u0012\b\b\t\u0012\u0004\b\b(\f\u0012\u0004\u0012\u00020\r\u0018\u00010\u0007j\u0004\u0018\u0001`\u000e¢\u0006\u0002\u0010\u000fJ\b\u0010\u0013\u001a\u00020\u000bH\u0016J\u001c\u0010\u0014\u001a\u00020\r2\n\u0010\u0015\u001a\u00060\u0002R\u00020\u00002\u0006\u0010\f\u001a\u00020\u000bH\u0016J\u001c\u0010\u0016\u001a\u00060\u0002R\u00020\u00002\u0006\u0010\u0017\u001a\u00020\u00182\u0006\u0010\u0019\u001a\u00020\u000bH\u0016J\u000e\u0010\u001a\u001a\u00020\r2\u0006\u0010\u001b\u001a\u00020\u000bR\u0014\u0010\u0003\u001a\b\u0012\u0004\u0012\u00020\u00050\u0004X\u0082\u0004¢\u0006\u0002\n\u0000RF\u0010\u0006\u001a:\u0012\u0013\u0012\u00110\u0005¢\u0006\f\b\b\u0012\b\b\t\u0012\u0004\b\b(\n\u0012\u0013\u0012\u00110\u000b¢\u0006\f\b\b\u0012\b\b\t\u0012\u0004\b\b(\f\u0012\u0004\u0012\u00020\r\u0018\u00010\u0007j\u0004\u0018\u0001`\u000eX\u0082\u0004¢\u0006\u0002\n\u0000R\u001c\u0010\u0010\u001a\u0010\u0012\u0004\u0012\u00020\u000b\u0012\u0006\u0012\u0004\u0018\u00010\u00120\u0011X\u0082\u0004¢\u0006\u0002\n\u0000¨\u0006\u001d"}, d2 = {"Lcom/blackhub/bronline/game/gui/familysystem/adapters/FamilyShopAdapter;", "Landroidx/recyclerview/widget/RecyclerView$Adapter;", "Lcom/blackhub/bronline/game/gui/familysystem/adapters/FamilyShopAdapter$ViewHolder;", "listShopItems", "", "Lcom/blackhub/bronline/game/gui/familysystem/data/FamilySystemShopObj;", "onClickItemInShopList", "Lkotlin/Function2;", "Lkotlin/ParameterName;", "name", "thisItem", "", "position", "", "Lcom/blackhub/bronline/game/gui/familysystem/OnClickItemInShopList;", "(Ljava/util/List;Lkotlin/jvm/functions/Function2;)V", "renderItem", "Landroidx/collection/ArrayMap;", "Landroid/graphics/Bitmap;", "getItemCount", "onBindViewHolder", "holder", "onCreateViewHolder", "parent", "Landroid/view/ViewGroup;", "viewType", "setCheckOnlyElement", "checkedPosition", "ViewHolder", "app_siteRelease"}, k = 1, mv = {1, 9, 0}, xi = 48)
@SourceDebugExtension({"SMAP\nFamilyShopAdapter.kt\nKotlin\n*S Kotlin\n*F\n+ 1 FamilyShopAdapter.kt\ncom/blackhub/bronline/game/gui/familysystem/adapters/FamilyShopAdapter\n+ 2 ArrayMap.kt\nandroidx/collection/ArrayMapKt\n*L\n1#1,116:1\n26#2:117\n*S KotlinDebug\n*F\n+ 1 FamilyShopAdapter.kt\ncom/blackhub/bronline/game/gui/familysystem/adapters/FamilyShopAdapter\n*L\n22#1:117\n*E\n"})
/* loaded from: classes3.dex */
public final class FamilyShopAdapter extends RecyclerView.Adapter<ViewHolder> {
    public static final int $stable = 8;

    @NotNull
    public final List<FamilySystemShopObj> listShopItems;

    @Nullable
    public final Function2<FamilySystemShopObj, Integer, Unit> onClickItemInShopList;

    @NotNull
    public final ArrayMap<Integer, Bitmap> renderItem;

    /* JADX WARN: Multi-variable type inference failed */
    public FamilyShopAdapter(@NotNull List<FamilySystemShopObj> listShopItems, @Nullable Function2<? super FamilySystemShopObj, ? super Integer, Unit> function2) {
        Intrinsics.checkNotNullParameter(listShopItems, "listShopItems");
        this.listShopItems = listShopItems;
        this.onClickItemInShopList = function2;
        this.renderItem = new ArrayMap<>();
    }

    @Override // androidx.recyclerview.widget.RecyclerView.Adapter
    @NotNull
    public ViewHolder onCreateViewHolder(@NotNull ViewGroup parent, int viewType) {
        Intrinsics.checkNotNullParameter(parent, "parent");
        FamiliesShopItemBinding inflate = FamiliesShopItemBinding.inflate(LayoutInflater.from(parent.getContext()), parent, false);
        Intrinsics.checkNotNullExpressionValue(inflate, "inflate(...)");
        return new ViewHolder(this, inflate);
    }

    @Override // androidx.recyclerview.widget.RecyclerView.Adapter
    public void onBindViewHolder(@NotNull ViewHolder holder, int position) {
        Intrinsics.checkNotNullParameter(holder, "holder");
        holder.bind(this.listShopItems.get(position), this.onClickItemInShopList);
    }

    @Override // androidx.recyclerview.widget.RecyclerView.Adapter
    public int getItemCount() {
        return this.listShopItems.size();
    }

    /* JADX WARN: Classes with same name are omitted:
  C:\javanew\classes3.dex
 */
    /* compiled from: FamilyShopAdapter.kt */
    @Metadata(d1 = {"\u00006\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0002\b\u0002\n\u0002\u0010\u0002\n\u0000\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0002\b\u0002\n\u0002\u0010\b\n\u0000\n\u0002\u0018\u0002\n\u0000\b\u0086\u0004\u0018\u00002\u00020\u0001B\r\u0012\u0006\u0010\u0002\u001a\u00020\u0003¢\u0006\u0002\u0010\u0004JN\u0010\u0005\u001a\u00020\u00062\u0006\u0010\u0007\u001a\u00020\b2>\u0010\t\u001a:\u0012\u0013\u0012\u00110\b¢\u0006\f\b\u000b\u0012\b\b\f\u0012\u0004\b\b(\r\u0012\u0013\u0012\u00110\u000e¢\u0006\f\b\u000b\u0012\b\b\f\u0012\u0004\b\b(\u000f\u0012\u0004\u0012\u00020\u0006\u0018\u00010\nj\u0004\u0018\u0001`\u0010R\u000e\u0010\u0002\u001a\u00020\u0003X\u0082\u0004¢\u0006\u0002\n\u0000¨\u0006\u0011"}, d2 = {"Lcom/blackhub/bronline/game/gui/familysystem/adapters/FamilyShopAdapter$ViewHolder;", "Landroidx/recyclerview/widget/RecyclerView$ViewHolder;", "binding", "Lcom/blackhub/bronline/databinding/FamiliesShopItemBinding;", "(Lcom/blackhub/bronline/game/gui/familysystem/adapters/FamilyShopAdapter;Lcom/blackhub/bronline/databinding/FamiliesShopItemBinding;)V", "bind", "", "item", "Lcom/blackhub/bronline/game/gui/familysystem/data/FamilySystemShopObj;", "onClickItemInShopList", "Lkotlin/Function2;", "Lkotlin/ParameterName;", "name", "thisItem", "", "position", "Lcom/blackhub/bronline/game/gui/familysystem/OnClickItemInShopList;", "app_siteRelease"}, k = 1, mv = {1, 9, 0}, xi = 48)
    public final class ViewHolder extends RecyclerView.ViewHolder {

        @NotNull
        public final FamiliesShopItemBinding binding;
        public final /* synthetic */ FamilyShopAdapter this$0;

        /* JADX WARN: 'super' call moved to the top of the method (can break code semantics) */
        public ViewHolder(@NotNull FamilyShopAdapter familyShopAdapter, FamiliesShopItemBinding binding) {
            super(binding.getRoot());
            Intrinsics.checkNotNullParameter(binding, "binding");
            this.this$0 = familyShopAdapter;
            this.binding = binding;
        }

        public final void bind(@NotNull final FamilySystemShopObj item, @Nullable final Function2<? super FamilySystemShopObj, ? super Integer, Unit> onClickItemInShopList) {
            Intrinsics.checkNotNullParameter(item, "item");
            FamiliesShopItemBinding familiesShopItemBinding = this.binding;
            FamilyShopAdapter familyShopAdapter = this.this$0;
            if (item.isClicked()) {
                familiesShopItemBinding.familyShopBgItem.setBackground(ResourcesCompat.getDrawable(familiesShopItemBinding.getRoot().getResources(), R.drawable.bg_families_shop_active_item, null));
            } else {
                familiesShopItemBinding.familyShopBgItem.setBackground(ResourcesCompat.getDrawable(familiesShopItemBinding.getRoot().getResources(), R.drawable.bg_families_shop_inactive_item, null));
            }
            familiesShopItemBinding.titleShopItem.setText(item.getTitle());
            if (item.getType() == 0) {
                familiesShopItemBinding.iconCurrency.setImageResource(R.drawable.ic_money_star);
            } else {
                familiesShopItemBinding.iconCurrency.setImageResource(R.drawable.ic_raw);
            }
            familiesShopItemBinding.valuePrice.setText(UsefulKt.getPriceWithDot(Integer.valueOf(item.getPrice())));
            if (item.getTypeId() != 3) {
                Bitmap bitmap = (Bitmap) familyShopAdapter.renderItem.get(Integer.valueOf(item.getObjectId()));
                if (bitmap == null) {
                    if (item.getObjectId() == 7378) {
                        int id = item.getId();
                        int objectId = item.getObjectId();
                        ImageView familyShopItemsIcon = familiesShopItemBinding.familyShopItemsIcon;
                        Intrinsics.checkNotNullExpressionValue(familyShopItemsIcon, "familyShopItemsIcon");
                        FamilyUtilsKt.renderShopItem(id, objectId, familyShopItemsIcon, familyShopAdapter.renderItem, 90.0f, 270.0f, 0.0f);
                    } else {
                        int id2 = item.getId();
                        int objectId2 = item.getObjectId();
                        ImageView familyShopItemsIcon2 = familiesShopItemBinding.familyShopItemsIcon;
                        Intrinsics.checkNotNullExpressionValue(familyShopItemsIcon2, "familyShopItemsIcon");
                        FamilyUtilsKt.renderShopItem(id2, objectId2, familyShopItemsIcon2, familyShopAdapter.renderItem, 20.0f, 180.0f, 0.0f);
                    }
                } else {
                    familiesShopItemBinding.familyShopItemsIcon.setImageBitmap(bitmap);
                }
            } else {
                familiesShopItemBinding.familyShopItemsIcon.setImageResource(R.drawable.tuning_icon_box);
            }
            familiesShopItemBinding.getRoot().setOnClickListener(new View.OnClickListener() { // from class: com.blackhub.bronline.game.gui.familysystem.adapters.FamilyShopAdapter$ViewHolder$$ExternalSyntheticLambda0
                @Override // android.view.View.OnClickListener
                public final void onClick(View view) {
                    FamilyShopAdapter.ViewHolder.bind$lambda$1$lambda$0(Function2.this, item, this, view);
                }
            });
        }

        public static final void bind$lambda$1$lambda$0(Function2 function2, FamilySystemShopObj item, ViewHolder this$0, View view) {
            Intrinsics.checkNotNullParameter(item, "$item");
            Intrinsics.checkNotNullParameter(this$0, "this$0");
            if (function2 != null) {
                function2.invoke(item, Integer.valueOf(this$0.getBindingAdapterPosition()));
            }
        }
    }

    public final void setCheckOnlyElement(int checkedPosition) {
        int size = this.listShopItems.size();
        for (int i = 0; i < size; i++) {
            if (this.listShopItems.get(i).isClicked() && i != checkedPosition) {
                this.listShopItems.get(i).setClicked(false);
                notifyItemChanged(i);
            }
        }
    }
}
