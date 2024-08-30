# fotafiles/models.py
from django.db import models
from django.conf import settings

class UploadedFile(models.Model):
    name = models.CharField(max_length=255, default='fota_file')
    file = models.FileField(upload_to='uploads/')  # Use 'uploads/' instead of 'media/uploads/'
    upload_date = models.DateTimeField(auto_now_add=True)
    content_type = models.CharField(max_length=100, default='application/octet-stream')
    uploaded_by = models.ForeignKey(settings.AUTH_USER_MODEL, on_delete=models.CASCADE, related_name='uploaded_files')

    def __str__(self):
        return self.name  # Display the file name in the admin

class Report(models.Model):
    username = models.CharField(max_length=255)
    report_text = models.TextField()
    date = models.DateTimeField(auto_now_add=True)
