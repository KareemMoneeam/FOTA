
from django.contrib import admin
from django.contrib.auth.admin import UserAdmin
from .models import CustomUser
from tester.admin_site import admin_site
class CustomUserAdmin(UserAdmin):
    model = CustomUser
    list_display = ('username', 'email', 'first_name', 'last_name', 'is_staff')
    search_fields = ('username', 'email', 'first_name', 'last_name')
    readonly_fields = ('date_joined', 'last_login')
    def reset_first_login_to_true(self, request, queryset):
        queryset.update(first_login=True)
    reset_first_login_to_true.short_description = "Reset QR Code"

    actions = [reset_first_login_to_true]
admin_site.register(CustomUser, CustomUserAdmin)


# Register your models here.
