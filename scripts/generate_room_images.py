#!/usr/bin/env python3
"""Generate images for Zork rooms using free text-to-image APIs."""

import re
import os
import sys
import time
import requests
import urllib.parse
import argparse
import subprocess
import shutil
from pathlib import Path

# Configuration
ROOMDEFS_FILE = "roomdefs.h"
OUTPUT_DIR = "zork_pics"
DELAY_BETWEEN_REQUESTS = 3  # seconds

# API Keys - Get your free token at https://huggingface.co/settings/tokens
HF_TOKEN = os.environ.get("HF_TOKEN", "")

# Style prompt suffix to make images look like classic text adventure game art
DEFAULT_STYLE = ", digital art, fantasy illustration, classic text adventure game style, atmospheric, detailed"


def chafa_available():
    """Check if chafa is installed on the system."""
    return shutil.which('chafa') is not None


def display_image_chafa(image_path, max_width=80, max_height=20):
    """Display an image using chafa in the terminal."""
    if not chafa_available():
        return False
    
    try:
        # Get terminal size
        cols = os.get_terminal_size().columns
        rows = os.get_terminal_size().lines
        
        # Use smaller of terminal size or max values
        width = min(cols - 4, max_width)
        height = min(rows - 8, max_height)
        
        # Run chafa
        result = subprocess.run(
            ['chafa', '--format', 'symbols', '--size', f'{width}x{height}', image_path],
            capture_output=True,
            text=True,
            timeout=10
        )
        
        if result.returncode == 0:
            print(result.stdout)
            return True
    except Exception as e:
        pass
    
    return False


def parse_roomdefs(filepath):
    """Parse roomdefs.h and extract room IDs and descriptions."""
    rooms = []
    
    with open(filepath, 'r') as f:
        content = f.read()
    
    # Pattern to match mr() macro calls
    # mr(R"~(ROOM_ID)~", R"~(DESCRIPTION)~", R"~(SHORT_NAME)~", ...
    pattern = r'mr\s*\(\s*R"~\(([^)]+)\)~"\s*,\s*R"~\(([^)]*)\)~"\s*,\s*R"~\(([^)]+)\)~"'
    
    matches = re.findall(pattern, content, re.DOTALL)
    
    for room_id, description, short_name in matches:
        # Clean up the data
        room_id = room_id.strip()
        description = description.strip()
        short_name = short_name.strip()
        
        # Skip rooms with empty descriptions (they use variables)
        if description and not description.startswith('st') and len(description) > 10:
            rooms.append({
                'id': room_id.lower(),
                'description': description,
                'name': short_name
            })
    
    return rooms


def generate_image_huggingface(prompt, output_path, token, style_suffix, max_retries=3):
    """Generate an image using Hugging Face Inference API (free tier)."""
    if not token:
        print("      Error: HF_TOKEN not set. Get a free token at https://huggingface.co/settings/tokens")
        print("      Set it with: export HF_TOKEN=your_token_here")
        return False
    
    full_prompt = prompt + style_suffix
    
    # Using Stable Diffusion XL - high quality, free tier available
    # New HF Inference endpoint (old api-inference.huggingface.co is deprecated)
    api_url = "https://router.huggingface.co/hf-inference/models/stabilityai/stable-diffusion-xl-base-1.0"
    
    headers = {
        "Authorization": f"Bearer {token}",
        "Content-Type": "application/json",
    }
    
    payload = {
        "inputs": full_prompt,
        "parameters": {
            "negative_prompt": "blurry, low quality, distorted, ugly, bad anatomy",
        }
    }
    
    for attempt in range(max_retries):
        try:
            print(f"      Attempt {attempt + 1}/{max_retries}...")
            response = requests.post(api_url, headers=headers, json=payload, timeout=120)
            
            if response.status_code == 200:
                # Check if response is an image
                content_type = response.headers.get('Content-Type', '')
                if 'image' in content_type:
                    with open(output_path, 'wb') as f:
                        f.write(response.content)
                    return True
                else:
                    # Might be JSON error
                    try:
                        error = response.json()
                        print(f"      API Error: {error.get('error', 'Unknown error')}")
                    except:
                        print(f"      Unexpected response: {content_type}")
            elif response.status_code == 503:
                # Model is loading, wait and retry
                print(f"      Model is loading, waiting...")
                if attempt < max_retries - 1:
                    time.sleep(20)
            elif response.status_code == 401:
                print(f"      Invalid HF_TOKEN. Get a free token at https://huggingface.co/settings/tokens")
                return False
            else:
                print(f"      HTTP {response.status_code}")
                print(f"      Response: {response.text}")
                
            if attempt < max_retries - 1:
                wait_time = 5
                print(f"      Retrying in {wait_time}s...")
                time.sleep(wait_time)
                
        except requests.exceptions.Timeout:
            print(f"      Request timed out")
            if attempt < max_retries - 1:
                time.sleep(10)
        except Exception as e:
            print(f"      Error: {e}")
            if attempt < max_retries - 1:
                time.sleep(5)
    
    return False


def generate_image(prompt, output_path, token, style_suffix, max_retries=3):
    """Generate an image using available free APIs."""
    return generate_image_huggingface(prompt, output_path, token, style_suffix, max_retries)


def main():
    """Main function to generate images for all rooms."""
    
    # Parse command line arguments
    parser = argparse.ArgumentParser(description='Generate images for Zork rooms')
    parser.add_argument('--limit', type=int, help='Limit number of images to generate')
    parser.add_argument('--room', type=str, help='Generate image for specific room ID only')
    parser.add_argument('--token', type=str, help='Hugging Face API token (or set HF_TOKEN env var)')
    parser.add_argument('--style', type=str, default=DEFAULT_STYLE, help='Style suffix to append to prompts')
    parser.add_argument('--no-display', action='store_true', help='Do not display images with chafa')
    parser.add_argument('--replace', action='store_true', help='Replace existing images without asking')
    parser.add_argument('--replace-ask', action='store_true', help='Ask before replacing existing images (shows old and new)')
    parser.add_argument('--list', action='store_true', help='List all rooms and their image status')
    parser.add_argument('--show', type=str, metavar='ROOM', help='Show a specific room image with chafa')
    args = parser.parse_args()
    
    # Get token: priority is argument > env var
    token = args.token or HF_TOKEN
    
    # Check if roomdefs.h exists
    if not os.path.exists(ROOMDEFS_FILE):
        print(f"Error: {ROOMDEFS_FILE} not found")
        print(f"Make sure you're running this script from the zork directory")
        sys.exit(1)
    
    # Create output directory if it doesn't exist
    os.makedirs(OUTPUT_DIR, exist_ok=True)
    
    # Parse room definitions
    rooms = parse_roomdefs(ROOMDEFS_FILE)
    
    # Check for existing images
    existing = {}
    for f in os.listdir(OUTPUT_DIR):
        if f.endswith(('.jpg', '.jpeg', '.png', '.gif')):
            room_id = os.path.splitext(f)[0].lower()
            existing[room_id] = os.path.join(OUTPUT_DIR, f)
    
    # Handle --list: show all rooms and their image status
    if args.list:
        print(f"{'ID':<15} {'NAME':<25} {'IMAGE':<10} {'DESCRIPTION'}")
        print("="*100)
        has_image = 0
        no_image = 0
        for room in rooms:
            img_status = "YES" if room['id'] in existing else "NO"
            if room['id'] in existing:
                has_image += 1
            else:
                no_image += 1
            desc = room['description'][:50] + "..." if len(room['description']) > 50 else room['description']
            print(f"{room['id']:<15} {room['name']:<25} {img_status:<10} {desc}")
        print("="*100)
        print(f"Total: {len(rooms)} rooms, {has_image} with images, {no_image} without images")
        return
    
    # Handle --show: display a specific room image
    if args.show:
        room_id = args.show.lower()
        if room_id not in existing:
            # Check if room exists at all
            room_names = [r['id'] for r in rooms]
            if room_id not in room_names:
                print(f"Error: Room '{args.show}' not found")
                print(f"Available rooms: {', '.join(sorted(room_names)[:20])}...")
            else:
                print(f"Room '{args.show}' exists but has no image")
                # Show room info
                for room in rooms:
                    if room['id'] == room_id:
                        print(f"  Name: {room['name']}")
                        print(f"  Description: {room['description']}")
            return
        
        image_path = existing[room_id]
        print(f"Room: {room_id}")
        for room in rooms:
            if room['id'] == room_id:
                print(f"Name: {room['name']}")
                print(f"Description: {room['description']}")
                break
        print(f"Image: {image_path}")
        print()
        
        if chafa_available():
            display_image_chafa(image_path)
        else:
            print("Chafa not installed - cannot display image")
        return
    
    # Check for token (only needed for generation)
    if not token:
        print("="*60)
        print("SETUP REQUIRED")
        print("="*60)
        print("You need a Hugging Face API token (free tier available).")
        print("")
        print("1. Go to: https://huggingface.co/settings/tokens")
        print("2. Create a free account if you don't have one")
        print("3. Generate a new token (read access is enough)")
        print("4. Either:")
        print("   - Run: export HF_TOKEN=your_token_here")
        print("   - Or pass: --token your_token_here")
        print("5. Run this script again")
        print("="*60)
        sys.exit(1)
    
    print(f"Parsing {ROOMDEFS_FILE}...")
    print(f"Found {len(rooms)} rooms with descriptions")
    
    # Show chafa status
    if chafa_available():
        print("Chafa: available (images will be displayed)")
    else:
        print("Chafa: not installed (images will not be displayed)")
    print()
    
    # Generate images
    generated = 0
    skipped = 0
    replaced = 0
    
    for room in rooms:
        room_id = room['id']
        
        # Filter by specific room if requested
        if args.room and room_id.lower() != args.room.lower():
            continue
        
        # Check if image already exists
        existing_path = existing.get(room_id)
        if existing_path and not args.replace and not args.replace_ask:
            print(f"[SKIP] {room_id}: {room['name']} (already exists)")
            skipped += 1
            continue
        
        # Limit check
        if args.limit and generated >= args.limit:
            print(f"\n  Reached limit of {args.limit} images")
            break
        
        # Determine output path
        output_path = os.path.join(OUTPUT_DIR, f"{room_id}.jpg")
        
        # Handle replace-ask: show existing image first
        if existing_path and args.replace_ask:
            print(f"\n[REPLACE?] {room_id}: {room['name']}")
            print(f"      Existing: {existing_path}")
            if chafa_available() and not args.no_display:
                print("\n  Existing image:")
                display_image_chafa(existing_path)
        
        print(f"\n[GEN] {room_id}: {room['name']}")
        print(f"      Description: {room['description']}")
        
        prompt = f"{room['description']}"
        
        # Generate new image to temp path first for replace-ask
        temp_path = output_path
        if existing_path and args.replace_ask:
            temp_path = os.path.join(OUTPUT_DIR, f"{room_id}_new.jpg")
        
        if generate_image(prompt, temp_path, token, args.style):
            # For replace-ask, show new image and ask
            if existing_path and args.replace_ask:
                if chafa_available() and not args.no_display:
                    print("\n  New image:")
                    display_image_chafa(temp_path)
                    print()
                
                while True:
                    print("\n  Options:")
                    print("    [y] Yes - replace existing image")
                    print("    [n] No - keep existing image")
                    print("    [r] Retry - regenerate with same prompt")
                    print("    [s] Style - modify style suffix and retry")
                    print("    [d] Description - add text to description and retry")
                    print("    [q] Quit - exit script")
                    response = input("\n  Your choice [y/n/r/s/d/q]: ").strip().lower()
                    
                    if response == 'y' or response == 'yes':
                        # Remove old and rename new
                        os.remove(existing_path)
                        if temp_path != output_path:
                            os.rename(temp_path, output_path)
                        print(f"      Replaced: {output_path}")
                        replaced += 1
                        break
                    elif response == 'n' or response == 'no':
                        # Remove the new temp image
                        if temp_path != output_path:
                            os.remove(temp_path)
                        print(f"      Kept existing image")
                        skipped += 1
                        break
                    elif response == 'r' or response == 'retry':
                        # Retry with same prompt
                        print("      Retrying...")
                        os.remove(temp_path)
                        if not generate_image(prompt, temp_path, token, args.style):
                            print("      Failed to generate image")
                        elif chafa_available() and not args.no_display:
                            print("\n  New image:")
                            display_image_chafa(temp_path)
                            print()
                        continue
                    elif response == 's' or response == 'style':
                        # Modify style and retry
                        new_style = input(f"      Current style: {args.style}\n      New style (or press Enter to keep): ").strip()
                        if new_style:
                            args.style = new_style
                        print(f"      Using style: {args.style}")
                        os.remove(temp_path)
                        if not generate_image(prompt, temp_path, token, args.style):
                            print("      Failed to generate image")
                        elif chafa_available() and not args.no_display:
                            print("\n  New image:")
                            display_image_chafa(temp_path)
                            print()
                        continue
                    elif response == 'd' or response == 'description':
                        # Add text to description
                        extra_text = input("      Additional text to add to description: ").strip()
                        if extra_text:
                            modified_prompt = prompt + " " + extra_text
                        else:
                            modified_prompt = prompt
                        print(f"      Modified prompt: {modified_prompt}")
                        os.remove(temp_path)
                        if not generate_image(modified_prompt, temp_path, token, args.style):
                            print("      Failed to generate image")
                        elif chafa_available() and not args.no_display:
                            print("\n  New image:")
                            display_image_chafa(temp_path)
                            print()
                        continue
                    elif response == 'q' or response == 'quit':
                        # Clean up and exit
                        if temp_path != output_path:
                            os.remove(temp_path)
                        print("\n  Exiting...")
                        sys.exit(0)
                    else:
                        print("      Invalid option, please try again")
            else:
                print(f"      Saved to: {output_path}")
                if existing_path and args.replace:
                    replaced += 1
                else:
                    generated += 1
            
            # Display the image with chafa if available and not disabled
            if not (existing_path and args.replace_ask) and chafa_available() and not args.no_display:
                print()
                display_image_chafa(output_path)
                print()
        else:
            print(f"      Failed to generate image")
        
        # Be nice to the API
        if generated > 0 or replaced > 0:
            print(f"  Pausing {DELAY_BETWEEN_REQUESTS}s...")
            time.sleep(DELAY_BETWEEN_REQUESTS)
    
    print(f"\n{'='*60}")
    print(f"Summary: Generated {generated} images, replaced {replaced} images, skipped {skipped} existing")
    print(f"Images saved to: {OUTPUT_DIR}/")


if __name__ == "__main__":
    main()