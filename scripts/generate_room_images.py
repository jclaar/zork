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
import tempfile
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


def edit_text_in_editor(initial_text, editor_hint="text"):
    """Open a text editor to edit the given text. Returns the edited text or None if cancelled."""
    # Try to find an editor
    editor = os.environ.get('EDITOR') or os.environ.get('VISUAL')
    
    # Fall back to common editors, Try text based first. As vim/vi is usually installed we check for some user installed first as if a user has
    # installed something like emacs, nano, micro etc. they probably prefer that over vim/vi which is often just there by default
    # e.g. the order is not in any "editor war" order.
    if not editor:
        for e in ['micro', 'nano', 'emacs', 'vim', 'vi', 'code', 'gedit']:
            if shutil.which(e):
                editor = e
                break
    
    if not editor:
        # No editor found, use simple inline editing
        print(f"\n  Current {editor_hint}:")
        print(f"  {initial_text}")
        print()
        new_text = input(f"  Enter new {editor_hint} (or press Enter to keep current): ").strip()
        return new_text if new_text else initial_text
    
    # Create temp file with initial content
    try:
        with tempfile.NamedTemporaryFile(mode='w', suffix='.txt', delete=False) as f:
            f.write(initial_text)
            temp_path = f.name
        
        # Open editor
        subprocess.run([editor, temp_path])
        
        # Read back edited content
        with open(temp_path, 'r') as f:
            edited_text = f.read().strip()
        
        # Clean up
        os.unlink(temp_path)
        
        return edited_text if edited_text else initial_text
    except Exception as e:
        print(f"  Error opening editor: {e}")
        return initial_text


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
        room_id = room_id.strip()
        description = description.strip()
        short_name = short_name.strip()
        
        # Skip rooms with empty descriptions - they use variables or dynamic descriptions
        if not description:
            continue
            
        # Skip rooms where description is a variable reference (starts with lowercase letter)
        if description[0].islower() and not description.startswith('"'):
            continue
            
        # Only include inline descriptions
        if len(description) > 10:
            rooms.append({
                'id': room_id.lower(),
                'description': description,
                'name': short_name,
                'source': 'inline'
            })
    
    return rooms


def parse_variable_descriptions(zstring_path):
    """Parse zstring.h for variable-based descriptions."""
    var_descriptions = {}
    
    if not os.path.exists(zstring_path):
        return var_descriptions
    
    with open(zstring_path, 'r') as f:
        content = f.read()
    
    # Match raw strings that may span multiple lines
    # Pattern: constexpr std::string_view  varname = R"~(content)~";
    # Use [\s\S]*? to match any character including newlines, non-greedy
    pattern = r'constexpr\s+std::string_view\s+(\w+)\s*=\s*R"~\(([\s\S]*?)\)~"'
    
    matches = re.findall(pattern, content)
    
    for varname, description in matches:
        varname = varname.strip()
        description = description.strip()
        if description:
            var_descriptions[varname] = description
    
    return var_descriptions
    
    with open(zstring_path, 'r') as f:
        content = f.read()
    
    pattern = r'constexpr\s+std::string_view\s+(\w+)\s*=\s*R"~\(([^)]*)\)~"'
    
    matches = re.findall(pattern, content, re.DOTALL)
    
    for varname, description in matches:
        varname = varname.strip()
        description = description.strip()
        if description:
            var_descriptions[varname] = description
    
    return var_descriptions


def parse_roomdefs_with_vars(filepath, zstring_path):
    """Parse roomdefs.h including rooms with variable descriptions.
    Returns a dict with room info including the room function name if present."""
    rooms = []
    room_func_map = {}
    
    with open(filepath, 'r') as f:
        content = f.read()
    
    # Get variable descriptions
    var_descriptions = parse_variable_descriptions(zstring_path)
    
    # Pattern to find the start of mr() - room_id
    pattern = r'mr\s*\(\s*R"~\(([^)]+)\)~"'
    
    for match in re.finditer(pattern, content):
        room_id = match.group(1).strip()
        start = match.end()
        
        # Parse the remaining arguments manually to handle multiline raw strings
        rest = content[start:]
        
        # Skip whitespace and comma
        rest = re.sub(r'^\s*,\s*', '', rest)
        
        # Parse description argument
        description = None
        source = None
        if rest.startswith('R"~('):
            # Raw string - find the matching )~"
            depth = 1
            i = 4
            while i < len(rest) and depth > 0:
                if rest[i:i+3] == ')~"':
                    depth -= 1
                    if depth == 0:
                        break
                elif rest[i:i+4] == 'R"~(':
                    depth += 1
                    i += 4
                    continue
                i += 1
            desc = rest[4:i].strip()
            remaining = rest[i+3:]
            if desc and len(desc) > 5:
                description = desc
                source = 'inline'
        else:
            # Variable - find the next comma
            comma_pos = rest.find(',')
            var_name = rest[:comma_pos].strip()
            remaining = rest[comma_pos+1:]
            if var_name in var_descriptions:
                description = var_descriptions[var_name]
                source = f'variable:{var_name}'
        
        # Skip whitespace and comma before short_name
        remaining = re.sub(r'^\s*,\s*', '', remaining)
        
        # Parse short_name argument
        short_name = ''
        if remaining.startswith('R"~('):
            # Raw string
            depth = 1
            i = 4
            while i < len(remaining) and depth > 0:
                if remaining[i:i+3] == ')~"':
                    depth -= 1
                    if depth == 0:
                        break
                elif remaining[i:i+4] == 'R"~(':
                    depth += 1
                    i += 4
                    continue
                i += 1
            short_name = remaining[4:i].strip()
            remaining = remaining[i+3:]
        else:
            # Variable - find the next comma
            comma_pos = remaining.find(',')
            var_name = remaining[:comma_pos].strip()
            remaining = remaining[comma_pos+1:]
            if var_name in var_descriptions:
                short_name = var_descriptions[var_name]
            else:
                short_name = var_name
        
        # Search for room_funcs:: in the remaining content
        func_match = re.search(r'room_funcs::(\w+)\s*\(\s*\)', remaining)
        if func_match:
            room_func_map[room_id.lower()] = func_match.group(1)
        
        rooms.append({
            'id': room_id.lower(),
            'description': description,
            'name': short_name,
            'source': source
        })
    
    return rooms, room_func_map


def parse_dynamic_descriptions(act_path, zstring_path):
    """Parse act files for dynamically generated room descriptions.
    Returns dict keyed by function name (not room_id)."""
    func_descriptions = {}
    
    # Get variable descriptions from zstring.h
    var_descriptions = parse_variable_descriptions(zstring_path)
    
    # Parse all act files (act1.cpp through act4.cpp)
    act_files = []
    if os.path.isdir(act_path):
        act_files = [os.path.join(act_path, f) for f in os.listdir(act_path) 
                     if f.startswith('act') and f.endswith('.cpp')]
    elif os.path.isfile(act_path):
        act_files = [act_path]
    else:
        # Try act1.cpp through act4.cpp in the same directory
        base_dir = os.path.dirname(act_path) or '.'
        act_files = [os.path.join(base_dir, f'act{i}.cpp') for i in range(1, 5)]
        act_files = [f for f in act_files if os.path.exists(f)]
    
    room_func_pattern = r'bool\s+(\w+)::operator\(\)\s*\(\s*\)\s*const'
    
    for act_file in act_files:
        if not os.path.exists(act_file):
            continue
            
        with open(act_file, 'r') as f:
            content = f.read()
        
        room_funcs = re.findall(room_func_pattern, content)
        
        for func_name in room_funcs:
            # Find the function with balanced braces
            func_start_pattern = rf'bool\s+{func_name}::operator\(\)\s*\(\s*\)\s*const\s*\{{'
            func_start_match = re.search(func_start_pattern, content)
            
            if not func_start_match:
                continue
            
            # Find matching closing brace by counting
            start = func_start_match.end()
            depth = 1
            pos = start
            while pos < len(content) and depth > 0:
                if content[pos] == '{':
                    depth += 1
                elif content[pos] == '}':
                    depth -= 1
                pos += 1
            
            func_body = content[start:pos-1]
            
            # Look for tell() calls with string literals (may be concatenated)
            # Handle: tell("string1" "string2" ...) or tell("string"...)
            tell_pattern = r'tell\s*\(\s*"([^"]*)"'
            tell_matches = re.findall(tell_pattern, func_body)
            
            # Also try to find tell() with the full first argument including concatenated strings
            full_tell_pattern = r'tell\s*\(\s*((?:"[^"]*"\s*)+)'
            full_tell_matches = re.findall(full_tell_pattern, func_body)
            
            # Combine all string parts from each match
            for strings_concat in full_tell_matches:
                # Extract all string parts
                parts = re.findall(r'"([^"]*)"', strings_concat)
                desc = ' '.join(p.strip() for p in parts if p.strip())
                desc = desc.replace('\\n', ' ').replace('\\t', ' ')
                if desc and len(desc) > 5:
                    func_descriptions[func_name] = {
                        'description': desc,
                        'source': f'function:{func_name}'
                    }
                    break
            
            # Also try simple tell patterns
            if func_name not in func_descriptions:
                for desc in tell_matches:
                    desc = desc.strip()
                    desc = desc.replace('\\n', ' ').replace('\\t', ' ')
                    if desc and len(desc) > 5:
                        func_descriptions[func_name] = {
                            'description': desc,
                            'source': f'function:{func_name}'
                        }
                        break
            
            # If not found, look for description variables in the function body
            if func_name not in func_descriptions:
                # Find all potential description variable names
                desc_vars = re.findall(r'\b(\w*_desc\w*)\b', func_body)
                desc_vars += re.findall(r'\b(\w*desc)\b', func_body)
                
                for var_name in desc_vars:
                    if var_name in var_descriptions:
                        desc = var_descriptions[var_name]
                        if len(desc) > 5:
                            func_descriptions[func_name] = {
                                'description': desc,
                                'source': f'zstring:{var_name}'
                            }
                            break
    
    return func_descriptions


def parse_roomdefs_complete(roomdefs_path, zstring_path, act_path):
    """Parse all room definitions from multiple sources."""
    all_rooms = {}
    
    # Get rooms from roomdefs.h with variable resolution and room function map
    rooms, room_func_map = parse_roomdefs_with_vars(roomdefs_path, zstring_path)
    
    # Get dynamic descriptions from act files
    func_descriptions = parse_dynamic_descriptions(act_path, zstring_path)
    
    # Build the final room list
    for room in rooms:
        room_id = room['id']
        
        # If no description, try to get from room function
        if not room['description'] and room_id in room_func_map:
            func_name = room_func_map[room_id]
            if func_name in func_descriptions:
                room['description'] = func_descriptions[func_name]['description']
                room['source'] = func_descriptions[func_name]['source']
        
        # Only include rooms with descriptions
        if room['description'] and len(room['description']) > 5:
            all_rooms[room_id] = room
    
    return list(all_rooms.values())


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
    rooms = parse_roomdefs_complete(ROOMDEFS_FILE, "zstring.h", ".")
    
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
                    print("    [r] Retry - regenerate with current prompt")
                    print("    [e] Edit - edit prompt in editor and regenerate")
                    print("    [q] Quit - exit script")
                    response = input("\n  Your choice [y/n/r/e/q]: ").strip().lower()
                    
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
                    elif response == 'e' or response == 'edit':
                        # Edit full prompt with editor
                        print("\n  Opening editor for prompt...")
                        full_prompt = prompt + " " + args.style
                        edited_prompt = edit_text_in_editor(full_prompt, "prompt")
                        print(f"      Using prompt: {edited_prompt}")
                        os.remove(temp_path)
                        if not generate_image(edited_prompt, temp_path, token, ""):
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