# tester/admin_site.py

from django_otp.admin import OTPAdminSite
from django.contrib.auth.models import User
from django_otp.plugins.otp_totp.models import TOTPDevice
from django_otp.plugins.otp_totp.admin import TOTPDeviceAdmin

class OTPAdmin(OTPAdminSite):
    pass

# Define the custom admin site
admin_site = OTPAdmin(name='OTPAdmin')
admin_site.register(User)
admin_site.register(TOTPDevice, TOTPDeviceAdmin)
