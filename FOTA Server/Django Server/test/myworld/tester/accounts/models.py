from django.contrib.auth.models import AbstractUser
from django.db import models
import pyotp
class CustomUser(AbstractUser):
    email = models.EmailField(unique=True)
    otp_secret = models.CharField(max_length=16, blank=True, null=True)
    first_login = models.BooleanField(default=True)
    
    def generate_otp_secret(self):
        self.otp_secret = pyotp.random_base32()
        return self.otp_secret

    def get_totp_uri(self):
        return pyotp.totp.TOTP(self.otp_secret).provisioning_uri(name=self.username, issuer_name="FOTA")
   