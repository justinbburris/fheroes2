#!/bin/bash

# Detect if we're using a dark background
if [ "$(tput colors)" -ge 256 ]; then
    # Get background color
    bg_color=$(tput longname | grep -o "bg=[0-9]*" | cut -d= -f2)
    
    # If background is dark (bg color is less than 128)
    if [ "$bg_color" -lt 128 ]; then
        # Light colors for dark background
        SUCCESS='\033[38;5;82m'  # Bright green
        ERROR='\033[38;5;203m'   # Soft red
        WARNING='\033[38;5;220m' # Bright yellow
        INFO='\033[38;5;75m'     # Light blue
    else
        # Dark colors for light background
        SUCCESS='\033[38;5;28m'  # Dark green
        ERROR='\033[38;5;160m'   # Dark red
        WARNING='\033[38;5;136m' # Dark yellow
        INFO='\033[38;5;27m'     # Dark blue
    fi
else
    # Fallback colors for terminals with limited color support
    SUCCESS='\033[1;32m'  # Bold green
    ERROR='\033[1;31m'    # Bold red
    WARNING='\033[1;33m'  # Bold yellow
    INFO='\033[1;34m'     # Bold blue
fi

NC='\033[0m' # No Color

# Get the project root directory (two directories up from the script)
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

echo -e "${WARNING}HOMM2 Folder Copy Script${NC}"
echo "This script will copy required HOMM2 folders to the fheroes2 root directory."
echo "Target directory: $PROJECT_ROOT"
echo ""

# Function to show directory contents
show_directory_contents() {
    local dir="$1"
    echo -e "\n${INFO}Contents of $dir:${NC}"
    echo "----------------------------------------"
    if [ -d "$dir" ]; then
        # List directories first
        echo -e "${INFO}Directories:${NC}"
        find "$dir" -maxdepth 1 -type d | sort | while read -r item; do
            if [ "$item" != "$dir" ]; then
                echo "  $(basename "$item")"
            fi
        done
        
        # List files
        echo -e "\n${INFO}Files:${NC}"
        find "$dir" -maxdepth 1 -type f | sort | while read -r item; do
            echo "  $(basename "$item")"
        done
    else
        echo -e "${ERROR}Directory does not exist${NC}"
    fi
    echo "----------------------------------------"
    echo ""
}

# Function to convert relative path to absolute path and expand tilde
get_absolute_path() {
    local path="$1"
    
    # Expand tilde to home directory
    path="${path/#\~/$HOME}"
    
    if [[ "$path" = /* ]]; then
        # Path is already absolute
        echo "$path"
    else
        # Convert relative path to absolute
        echo "$(cd "$(dirname "$path")" && pwd)/$(basename "$path")"
    fi
}

# Prompt for HOMM2 folder
while true; do
    read -e -p "Please enter the path to your HOMM2 folder: " homm2_path
    
    # Convert to absolute path
    homm2_path_abs=$(get_absolute_path "$homm2_path")
    
    # Show preview of the directory
    show_directory_contents "$homm2_path_abs"
    
    # Ask for confirmation
    read -p "Is this the correct HOMM2 folder? (y/n): " confirm
    if [[ $confirm == [yY] || $confirm == [yY][eE][sS] ]]; then
        break
    fi
    # Clear the screen
    echo -e "\033[2J\033[H"
    echo -e "${WARNING}HOMM2 Folder Copy Script${NC}"
    echo "This script will copy required HOMM2 folders to the fheroes2 root directory."
    echo "Target directory: $PROJECT_ROOT"
    echo ""
done

# Check if the path exists
if [ ! -d "$homm2_path_abs" ]; then
    echo -e "${ERROR}Error: The specified HOMM2 folder does not exist.${NC}"
    exit 1
fi

# Array of required folders
folders=("ANIM" "DATA" "MAPS" "MUSIC")
missing_folders=()
copied_folders=()

# Check which folders exist and copy them
for folder in "${folders[@]}"; do
    if [ -d "$homm2_path_abs/$folder" ]; then
        echo -e "${WARNING}Copying $folder...${NC}"
        if cp -R "$homm2_path_abs/$folder" "$PROJECT_ROOT/"; then
            echo -e "${SUCCESS}Successfully copied $folder${NC}"
            copied_folders+=("$folder")
        else
            echo -e "${ERROR}Failed to copy $folder${NC}"
        fi
    else
        echo -e "${WARNING}Folder $folder not found, skipping...${NC}"
        missing_folders+=("$folder")
    fi
done

# Show summary
echo -e "\n${SUCCESS}Copy operation completed!${NC}"
echo "The following folders were copied to the fheroes2 root directory:"
if [ ${#copied_folders[@]} -eq 0 ]; then
    echo "  None"
else
    for folder in "${copied_folders[@]}"; do
        echo "  - $folder"
    done
fi

echo -e "\n${WARNING}The following folders were not found and were skipped:${NC}"
if [ ${#missing_folders[@]} -eq 0 ]; then
    echo "  None"
else
    for folder in "${missing_folders[@]}"; do
        echo "  - $folder"
    done
fi 