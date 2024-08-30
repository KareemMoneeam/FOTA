from django.shortcuts import render
from django.contrib.auth.decorators import login_required
from django.contrib.auth import logout
from django.shortcuts import redirect

@login_required
def index(request):
    var = {'id': 2020025, 'name': 'kick'}
    return render(request, 'pages/index.html', var)

@login_required
def about(request):
    return render(request, 'pages/about.html')

def logout_view(request):
    logout(request)
    return redirect('login')  