# Generated by Django 5.0.3 on 2024-06-22 17:21

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('fotafiles', '0007_rename_received_datetime_report_date_and_more'),
    ]

    operations = [
        migrations.AlterField(
            model_name='uploadedfile',
            name='file',
            field=models.FileField(upload_to='media/uploads/'),
        ),
    ]