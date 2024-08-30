# fotafiles/admin.py

from django.contrib import admin
from .models import UploadedFile, Report

# Import custom admin_site from admin_site.py
from tester.admin_site import admin_site  # Adjust 'tester' if it’s not the project directory name

class UploadedFileAdmin(admin.ModelAdmin):
    list_display = ('name', 'upload_date', 'get_uploaded_by')

    def get_uploaded_by(self, obj):
        return obj.uploaded_by.username if obj.uploaded_by else "None"
    
    get_uploaded_by.short_description = 'Uploaded By'

# Register UploadedFile with custom admin_site
admin_site.register(UploadedFile, UploadedFileAdmin)

class ReportAdmin(admin.ModelAdmin):
    list_display = ('username', 'report_text', 'date')
    search_fields = ('username', 'report_text')

# Ensure Report is registered with custom admin_site
admin_site.register(Report, ReportAdmin)
