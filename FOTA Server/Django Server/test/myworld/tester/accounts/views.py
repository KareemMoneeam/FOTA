from django.shortcuts import render, redirect
from django.contrib.auth.forms import AuthenticationForm
from django.contrib.auth import authenticate, login as auth_login
from django.contrib.auth.decorators import login_required
from .forms import OTPForm
from .forms import RegistrationForm
from pyotp import TOTP
from .models import CustomUser
from .forms import LoginForm
import base64
import pyotp
import qrcode
from io import BytesIO


def login_view(request):
    if request.user.is_authenticated:
        return redirect('index')
    
    if request.method == 'POST':
        form = AuthenticationForm(request, data=request.POST)
        if form.is_valid():
            username = form.cleaned_data.get('username')
            password = form.cleaned_data.get('password')
            user = authenticate(username=username, password=password)
            if user is not None:
                auth_login(request, user)
                
                # Check if it's the first login
                if user.first_login:
                    otp_secret = user.generate_otp_secret()
                    user.first_login = False
                    user.save()
                    
                    # Generate QR code
                    qr_code = generate_qr_code(username, otp_secret)
                    
                    # Redirect to OTP verification page with QR code
                    return render(request, 'accounts/otp.html', {'qr_code': qr_code})
                else:
                    return redirect('otp')  # Redirect to OTP verification page without QR code
            else:
                return render(request, 'accounts/login.html', {'form': form, 'invalid_creds': True})
    else:
        form = AuthenticationForm()
    
    return render(request, 'accounts/login.html', {'form': form})

def generate_qr_code(username, otp_secret):
    # Generate TOTP URI for the QR code
    totp_uri = pyotp.totp.TOTP(otp_secret).provisioning_uri(name=username, issuer_name="YourApp")
    
    # Generate QR code image
    img = qrcode.make(totp_uri)
    
    # Save QR code to BytesIO buffer
    buffer = BytesIO()
    img.save(buffer, format="PNG")
    
    # Encode image to base64
    qr_code = base64.b64encode(buffer.getvalue()).decode()
    
    return qr_code

def register_view(request):
    if request.method == 'POST':
        form = RegistrationForm(request.POST)
        if form.is_valid():
            form.save()
            return redirect('login')
    else:
        form = RegistrationForm()
    return render(request, 'accounts/register.html', {'form': form})


@login_required
def otp_view(request):
    user = request.user  # Assuming the user is logged in
    form = OTPForm()

    if request.method == 'POST':
        form = OTPForm(request.POST)
        if form.is_valid():
            otp = form.cleaned_data.get('otp')

            # Retrieve the user's OTP secret from the database
            otp_secret = user.otp_secret  # Assuming you have stored otp_secret in your CustomUser model

            # Verify the OTP
            totp = TOTP(otp_secret)
            if totp.verify(otp):
               
                return redirect('index')
            else:
                # OTP verification failed
                form.add_error('otp', 'Invalid OTP')

    return render(request, 'accounts/otp.html', {'form': form})