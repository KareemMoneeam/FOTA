# Generated by Django 5.0.3 on 2024-05-19 23:38

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('fotafiles', '0005_uploadedfile_content_type_uploadedfile_name'),
    ]

    operations = [
        migrations.CreateModel(
            name='Report',
            fields=[
                ('id', models.BigAutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('username', models.CharField(max_length=100)),
                ('report_text', models.TextField()),
                ('received_datetime', models.DateTimeField(auto_now_add=True)),
            ],
        ),
    ]
