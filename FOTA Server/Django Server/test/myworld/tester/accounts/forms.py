# accounts/forms.py
from django import forms
from django.contrib.auth.forms import UserCreationForm
from .models import CustomUser
from django.contrib.auth.forms import AuthenticationForm

class RegistrationForm(UserCreationForm):
    email = forms.EmailField(max_length=254, help_text='Required. Enter a valid email address.')

    class Meta:
        model = CustomUser
        fields = ['username', 'email', 'password1', 'password2']

# accounts/forms.py
from django import forms


class LoginForm(AuthenticationForm):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.fields['username'].widget.attrs.update({'placeholder': 'Enter your username'})
        self.fields['password'].widget.attrs.update({'placeholder': 'Enter your password'})

# forms.py
from django import forms

class OTPForm(forms.Form):
    otp = forms.CharField(max_length=6, required=True, widget=forms.TextInput(attrs={'placeholder': 'Enter OTP'}))
