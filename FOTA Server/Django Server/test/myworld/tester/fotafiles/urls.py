from django.urls import path
from django.contrib.auth.decorators import login_required
from . import views

urlpatterns = [
    path('upload/', login_required(views.upload), name='upload'),
    path('dashboard/', login_required(views.dashboard), name='dashboard'),
    # Ensure this is for deletion and clearly named
    #path('file/<int:file_id>/delete/', login_required(views.delete_file), name='delete_file'),
    path('file/<int:file_id>/download/', login_required(views.file_download), name='file_download'),
   
    path('send-file/<int:file_id>/', views.send_file_to_external_server, name='send_file'),
    path('receive-success/', views.receive_success, name='receive_success'),
    path('reports/', views.reports, name='reports'),
    path('reports/reports_incomeing', views.reports_incomeing, name='reports_incomeing'),
]