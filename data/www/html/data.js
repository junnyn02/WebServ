let galleryContainer, leftArrow, rightArrow, addBox;
let cardWidth = 150 + 16; // largeur image + gap
let scrollPosition = 0;

// 1. Gestion affichage si aucune image (seul le + visible)
function updateGalleryVisibility() {
  const imageBoxes = galleryContainer.querySelectorAll('.image-box:not(.add-box)');
  if (imageBoxes.length === 0) {
    imageBoxes.forEach(box => box.style.display = 'none');
    addBox.style.display = 'flex';
    leftArrow.style.display = 'none';
    rightArrow.style.display = 'none';
    galleryContainer.style.width = '150px'; // juste le +
    galleryContainer.style.overflowX = 'visible';
  } else {
    imageBoxes.forEach(box => box.style.display = 'flex');
    addBox.style.display = 'flex';
    leftArrow.style.display = 'inline-block';
    rightArrow.style.display = 'inline-block';
    galleryContainer.style.width = `calc(150px * 4 + 16px * 3)`;
    galleryContainer.style.overflowX = 'hidden';
  }
}

// 2. Gestion flèches et scroll seulement si plus de 4 carrés (images + +)
function updateArrows() {
  const totalCards = galleryContainer.querySelectorAll('.image-box').length;
  if (totalCards <= 4) {
    leftArrow.disabled = true;
    rightArrow.disabled = true;
  } else {
    leftArrow.disabled = scrollPosition <= 0;
    rightArrow.disabled = scrollPosition >= (galleryContainer.scrollWidth - galleryContainer.clientWidth);
  }
}

// Fonction pour supprimer une image (envoi requête DELETE)
async function deleteImage(imageUrl, imageBox) {
  try {
    const response = await fetch('/delete', {
      method: 'DELETE',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ url: imageUrl })
    });

    if (!response.ok) throw new Error('Erreur suppression image');

    // Supprime la boîte dans la galerie
    imageBox.remove();

    // Mise à jour affichage
    updateGalleryVisibility();
    updateArrows();

    console.log('Image supprimée:', imageUrl);
  } catch (error) {
    console.error('Erreur suppression image:', error);
    alert('Échec de la suppression : ' + error.message);
  }
}

// Chargement des images déjà uploadées depuis le serveur
async function loadUploadedImages() {
  try {
    const response = await fetch('/list-uploads'); // appelle ton serveur C++
    if (!response.ok) throw new Error('Erreur chargement des images');

    const images = await response.json(); // ex: ["/upload/img1.jpg", "/upload/img2.png"]

    galleryContainer = document.getElementById('gallery-container');
    addBox = galleryContainer.querySelector('.add-box');

    images.forEach(imageUrl => {
      const box = document.createElement('div');
      box.className = 'image-box';

      const img = document.createElement('img');
      img.src = imageUrl;
      img.alt = "Image";

      // Bouton supprimer
      const deleteBtn = document.createElement('button');
      deleteBtn.textContent = '×';
      deleteBtn.className = 'delete-btn';
      deleteBtn.title = 'Supprimer cette image';

      deleteBtn.addEventListener('click', () => {
        deleteImage(imageUrl, box);
      });

      box.appendChild(img);
      box.appendChild(deleteBtn);
      galleryContainer.insertBefore(box, addBox); // insère avant le "+"
    });

    updateGalleryVisibility();
    updateArrows();
  } catch (error) {
    console.error("Erreur lors du chargement des images :", error);
  }
}

document.addEventListener('DOMContentLoaded', () => {
  galleryContainer = document.getElementById('gallery-container');
  leftArrow = document.querySelector('.arrow.left');
  rightArrow = document.querySelector('.arrow.right');
  addBox = galleryContainer.querySelector('.add-box');

  loadUploadedImages();

  updateGalleryVisibility();
  updateArrows();

  leftArrow.addEventListener('click', () => {
    scrollPosition = Math.max(scrollPosition - cardWidth, 0);
    galleryContainer.scrollTo({ left: scrollPosition, behavior: 'smooth' });
    updateArrows();
  });

  rightArrow.addEventListener('click', () => {
    scrollPosition = Math.min(scrollPosition + cardWidth, galleryContainer.scrollWidth - galleryContainer.clientWidth);
    galleryContainer.scrollTo({ left: scrollPosition, behavior: 'smooth' });
    updateArrows();
  });

  // Gestion upload d'image
  const fileInput = document.getElementById("fileInput");
  fileInput.addEventListener('change', async (event) => {
    const file = event.target.files[0];
    if (!file) return;

    const formData = new FormData();
    formData.append('image', file);

    try {
      const response = await fetch('/upload', {
        method: 'POST',
        body: formData,
      });

      if (!response.ok) throw new Error('Erreur lors de l\'upload');

      const jsonResponse = await response.json();
      const imageUrl = jsonResponse.url;
      console.log('Image enregistrée:', imageUrl);

      const newImageBox = document.createElement('div');
      newImageBox.classList.add('image-box');

      const img = document.createElement('img');
      img.src = imageUrl;
      img.alt = file.name;

      const deleteBtn = document.createElement('button');
      deleteBtn.textContent = '×';
      deleteBtn.className = 'delete-btn';
      deleteBtn.title = 'Supprimer cette image';

      deleteBtn.addEventListener('click', () => {
        deleteImage(imageUrl, newImageBox);
      });

      newImageBox.appendChild(img);
      newImageBox.appendChild(deleteBtn);
      galleryContainer.insertBefore(newImageBox, addBox);

      fileInput.value = '';

      updateGalleryVisibility();
      updateArrows();
    } catch (error) {
      console.error('Upload échoué:', error);
      alert('Échec de l\'upload : ' + error.message);
    }
  });
});

// --- Fonctions pour charger les autres données du portfolio ---
async function loadPortfolioData() {
  try {
    const response = await fetch('portfolio.json');
    if (!response.ok) throw new Error(`Erreur HTTP: ${response.status}`);

    const data = await response.json();
    console.log('Données chargées:', data);

    updatePersonalInfo(data.personal);
    displaySkills(data.skills);
    showProjects(data.projects);
  } catch (error) {
    console.error('Erreur lors du chargement des données:', error);
    const skillsContainer = document.getElementById('skills-container');
    if (skillsContainer) {
      skillsContainer.innerHTML = `<p style="color: red;">Erreur: ${error.message}</p>`;
    }
  }
}

function updatePersonalInfo(personal) {
  if (!personal) return;
  const h1 = document.querySelector('h1');
  if (h1) h1.textContent = `Welcome to ${personal.name}'s Portfolio`;
}

function displaySkills(skills) {
  const skillsContainer = document.getElementById('skills-container');
  if (!skillsContainer) return;

  if (!skills || !Array.isArray(skills)) {
    skillsContainer.innerHTML = '<p>Aucune compétence à afficher</p>';
    return;
  }

  skillsContainer.innerHTML = '<h2>Mes Compétences</h2>';

  skills.forEach(skill => {
    const skillDiv = document.createElement('div');
    skillDiv.className = 'skill-item';

    const skillName = document.createElement('h3');
    skillName.textContent = skill.name;

    const skillInfo = document.createElement('p');
    skillInfo.textContent = `Niveau: ${skill.level}% | Catégorie: ${skill.category}`;

    const barBackground = document.createElement('div');
    barBackground.className = 'skill-bar-background';

    const barFill = document.createElement('div');
    barFill.className = 'skill-bar-fill';
    barFill.style.width = `${skill.level}%`;

    barBackground.appendChild(barFill);

    skillDiv.appendChild(skillName);
    skillDiv.appendChild(skillInfo);
    skillDiv.appendChild(barBackground);

    skillsContainer.appendChild(skillDiv);
  });
}

function showProjects(projects) {
  const projectsContainer = document.getElementById('projects-container');
  if (!projectsContainer) return;

  if (!projects || !Array.isArray(projects)) {
    projectsContainer.innerHTML = '<p>Aucun projet à afficher</p>';
    return;
  }

  projectsContainer.innerHTML = '<h2>Mes Projets</h2>';

  projects.forEach(project => {
    const projectDiv = document.createElement('div');
    projectDiv.className = 'project-item';

    const techList = project.technologies ? project.technologies.join(', ') : 'Non spécifié';

    projectDiv.innerHTML = `
      <h3>${project.name}</h3>
      <p>${project.description}</p>
      <p><strong>Technologies:</strong> ${techList}</p>
      <p><strong>Statut:</strong> ${project.status}</p>
    `;

    projectsContainer.appendChild(projectDiv);
  });
}

// Charger les données portfolio au chargement de la page
document.addEventListener('DOMContentLoaded', loadPortfolioData);
