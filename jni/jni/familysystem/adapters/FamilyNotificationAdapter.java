package com.blackhub.bronline.game.gui.familysystem.adapters;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.animation.AnimationUtils;
import android.widget.TextView;
import androidx.compose.runtime.internal.StabilityInferred;
import androidx.recyclerview.widget.RecyclerView;
import com.blackhub.bronline.R;
import com.blackhub.bronline.databinding.FamiliesNotificationItemBinding;
import com.blackhub.bronline.game.core.JNIActivity;
import com.blackhub.bronline.game.gui.familysystem.adapters.FamilyNotificationAdapter;
import com.blackhub.bronline.game.gui.familysystem.data.FamilyNotificationData;
import java.util.List;
import kotlin.Metadata;
import kotlin.Unit;
import kotlin.jvm.functions.Function1;
import kotlin.jvm.internal.Intrinsics;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

/* JADX WARN: Classes with same name are omitted:
  C:\javanew\classes3.dex
 */
/* compiled from: FamilyNotificationAdapter.kt */
@StabilityInferred(parameters = 0)
@Metadata(d1 = {"\u0000F\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0000\n\u0002\u0010 \n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0002\b\u0002\n\u0002\u0010\u0002\n\u0002\u0018\u0002\n\u0002\b\u0002\n\u0002\u0010\b\n\u0002\b\u0005\n\u0002\u0018\u0002\n\u0002\b\u0003\b\u0007\u0018\u00002\f\u0012\b\u0012\u00060\u0002R\u00020\u00000\u0001:\u0001\u0019BH\u0012\f\u0010\u0003\u001a\b\u0012\u0004\u0012\u00020\u00050\u0004\u0012\b\u0010\u0006\u001a\u0004\u0018\u00010\u0007\u0012)\u0010\b\u001a%\u0012\u0013\u0012\u00110\u0005¢\u0006\f\b\n\u0012\b\b\u000b\u0012\u0004\b\b(\f\u0012\u0004\u0012\u00020\r\u0018\u00010\tj\u0004\u0018\u0001`\u000e¢\u0006\u0002\u0010\u000fJ\b\u0010\u0010\u001a\u00020\u0011H\u0016J\u001c\u0010\u0012\u001a\u00020\r2\n\u0010\u0013\u001a\u00060\u0002R\u00020\u00002\u0006\u0010\u0014\u001a\u00020\u0011H\u0016J\u001c\u0010\u0015\u001a\u00060\u0002R\u00020\u00002\u0006\u0010\u0016\u001a\u00020\u00172\u0006\u0010\u0018\u001a\u00020\u0011H\u0016R1\u0010\b\u001a%\u0012\u0013\u0012\u00110\u0005¢\u0006\f\b\n\u0012\b\b\u000b\u0012\u0004\b\b(\f\u0012\u0004\u0012\u00020\r\u0018\u00010\tj\u0004\u0018\u0001`\u000eX\u0082\u0004¢\u0006\u0002\n\u0000R\u0014\u0010\u0003\u001a\b\u0012\u0004\u0012\u00020\u00050\u0004X\u0082\u0004¢\u0006\u0002\n\u0000R\u0010\u0010\u0006\u001a\u0004\u0018\u00010\u0007X\u0082\u0004¢\u0006\u0002\n\u0000¨\u0006\u001a"}, d2 = {"Lcom/blackhub/bronline/game/gui/familysystem/adapters/FamilyNotificationAdapter;", "Landroidx/recyclerview/widget/RecyclerView$Adapter;", "Lcom/blackhub/bronline/game/gui/familysystem/adapters/FamilyNotificationAdapter$ViewHolder;", "familyNotifications", "", "Lcom/blackhub/bronline/game/gui/familysystem/data/FamilyNotificationData;", "mainActivity", "Lcom/blackhub/bronline/game/core/JNIActivity;", "clickNotification", "Lkotlin/Function1;", "Lkotlin/ParameterName;", "name", "thisNotification", "", "Lcom/blackhub/bronline/game/gui/familysystem/OnClickNotification;", "(Ljava/util/List;Lcom/blackhub/bronline/game/core/JNIActivity;Lkotlin/jvm/functions/Function1;)V", "getItemCount", "", "onBindViewHolder", "holder", "position", "onCreateViewHolder", "parent", "Landroid/view/ViewGroup;", "viewType", "ViewHolder", "app_siteRelease"}, k = 1, mv = {1, 9, 0}, xi = 48)
/* loaded from: classes3.dex */
public final class FamilyNotificationAdapter extends RecyclerView.Adapter<ViewHolder> {
    public static final int $stable = 8;

    @Nullable
    public final Function1<FamilyNotificationData, Unit> clickNotification;

    @NotNull
    public final List<FamilyNotificationData> familyNotifications;

    @Nullable
    public final JNIActivity mainActivity;

    /* JADX WARN: Multi-variable type inference failed */
    public FamilyNotificationAdapter(@NotNull List<FamilyNotificationData> familyNotifications, @Nullable JNIActivity jNIActivity, @Nullable Function1<? super FamilyNotificationData, Unit> function1) {
        Intrinsics.checkNotNullParameter(familyNotifications, "familyNotifications");
        this.familyNotifications = familyNotifications;
        this.mainActivity = jNIActivity;
        this.clickNotification = function1;
    }

    @Override // androidx.recyclerview.widget.RecyclerView.Adapter
    @NotNull
    public ViewHolder onCreateViewHolder(@NotNull ViewGroup parent, int viewType) {
        Intrinsics.checkNotNullParameter(parent, "parent");
        FamiliesNotificationItemBinding inflate = FamiliesNotificationItemBinding.inflate(LayoutInflater.from(parent.getContext()), parent, false);
        Intrinsics.checkNotNullExpressionValue(inflate, "inflate(...)");
        return new ViewHolder(this, inflate);
    }

    @Override // androidx.recyclerview.widget.RecyclerView.Adapter
    public void onBindViewHolder(@NotNull ViewHolder holder, int position) {
        Intrinsics.checkNotNullParameter(holder, "holder");
        holder.bind(this.familyNotifications.get(position), this.clickNotification);
    }

    @Override // androidx.recyclerview.widget.RecyclerView.Adapter
    public int getItemCount() {
        return this.familyNotifications.size();
    }

    /* JADX WARN: Classes with same name are omitted:
  C:\javanew\classes3.dex
 */
    /* compiled from: FamilyNotificationAdapter.kt */
    @Metadata(d1 = {"\u00000\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0002\b\u0002\n\u0002\u0010\u0002\n\u0000\n\u0002\u0018\u0002\n\u0000\n\u0002\u0018\u0002\n\u0002\u0018\u0002\n\u0002\b\u0002\n\u0002\u0018\u0002\n\u0000\b\u0086\u0004\u0018\u00002\u00020\u0001B\r\u0012\u0006\u0010\u0002\u001a\u00020\u0003¢\u0006\u0002\u0010\u0004J9\u0010\u0005\u001a\u00020\u00062\u0006\u0010\u0007\u001a\u00020\b2)\u0010\t\u001a%\u0012\u0013\u0012\u00110\b¢\u0006\f\b\u000b\u0012\b\b\f\u0012\u0004\b\b(\r\u0012\u0004\u0012\u00020\u0006\u0018\u00010\nj\u0004\u0018\u0001`\u000eR\u000e\u0010\u0002\u001a\u00020\u0003X\u0082\u0004¢\u0006\u0002\n\u0000¨\u0006\u000f"}, d2 = {"Lcom/blackhub/bronline/game/gui/familysystem/adapters/FamilyNotificationAdapter$ViewHolder;", "Landroidx/recyclerview/widget/RecyclerView$ViewHolder;", "binding", "Lcom/blackhub/bronline/databinding/FamiliesNotificationItemBinding;", "(Lcom/blackhub/bronline/game/gui/familysystem/adapters/FamilyNotificationAdapter;Lcom/blackhub/bronline/databinding/FamiliesNotificationItemBinding;)V", "bind", "", "notification", "Lcom/blackhub/bronline/game/gui/familysystem/data/FamilyNotificationData;", "clickNotification", "Lkotlin/Function1;", "Lkotlin/ParameterName;", "name", "thisNotification", "Lcom/blackhub/bronline/game/gui/familysystem/OnClickNotification;", "app_siteRelease"}, k = 1, mv = {1, 9, 0}, xi = 48)
    public final class ViewHolder extends RecyclerView.ViewHolder {

        @NotNull
        public final FamiliesNotificationItemBinding binding;
        public final /* synthetic */ FamilyNotificationAdapter this$0;

        /* JADX WARN: 'super' call moved to the top of the method (can break code semantics) */
        public ViewHolder(@NotNull FamilyNotificationAdapter familyNotificationAdapter, FamiliesNotificationItemBinding binding) {
            super(binding.getRoot());
            Intrinsics.checkNotNullParameter(binding, "binding");
            this.this$0 = familyNotificationAdapter;
            this.binding = binding;
        }

        public final void bind(@NotNull final FamilyNotificationData notification, @Nullable final Function1<? super FamilyNotificationData, Unit> clickNotification) {
            String str;
            Intrinsics.checkNotNullParameter(notification, "notification");
            final FamiliesNotificationItemBinding familiesNotificationItemBinding = this.binding;
            FamilyNotificationAdapter familyNotificationAdapter = this.this$0;
            familiesNotificationItemBinding.notificationsValue.setText(notification.getNotificationsValue());
            TextView textView = familiesNotificationItemBinding.notificationsAuthor;
            JNIActivity jNIActivity = familyNotificationAdapter.mainActivity;
            if (jNIActivity == null || (str = jNIActivity.getString(R.string.families_notifications_author, notification.getNotificationsAuthor())) == null) {
                str = "";
            }
            textView.setText(str);
            familiesNotificationItemBinding.getRoot().setOnClickListener(new View.OnClickListener() { // from class: com.blackhub.bronline.game.gui.familysystem.adapters.FamilyNotificationAdapter$ViewHolder$$ExternalSyntheticLambda0
                @Override // android.view.View.OnClickListener
                public final void onClick(View view) {
                    FamilyNotificationAdapter.ViewHolder.bind$lambda$1$lambda$0(FamiliesNotificationItemBinding.this, clickNotification, notification, view);
                }
            });
        }

        public static final void bind$lambda$1$lambda$0(FamiliesNotificationItemBinding this_apply, Function1 function1, FamilyNotificationData notification, View view) {
            Intrinsics.checkNotNullParameter(this_apply, "$this_apply");
            Intrinsics.checkNotNullParameter(notification, "$notification");
            view.startAnimation(AnimationUtils.loadAnimation(this_apply.getRoot().getContext(), R.anim.button_click));
            if (function1 != null) {
                function1.invoke(notification);
            }
        }
    }
}
