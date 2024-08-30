from django.urls import path
from django.contrib.auth.decorators import login_required
from . import views

urlpatterns = [
    path('', login_required(views.index), name='index'),
    path('about/', login_required(views.about), name='about'),
    path('logout/', views.logout_view, name='logout'),
    
]
