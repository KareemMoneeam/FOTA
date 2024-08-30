from django.urls import path
from . import views

urlpatterns=[
    path('',views.login_view,name='login'),
    path('register/', views.register_view, name='register'),
    path('otp/', views.otp_view, name='otp'),
]