document.addEventListener('DOMContentLoaded', () => {
  const galleryContainer = document.getElementById('gallery-container');
  const leftArrow = document.querySelector('.arrow.left');
  const rightArrow = document.querySelector('.arrow.right');
  const addBox = galleryContainer.querySelector('.add-box');

  const cardWidth = 150 + 16; // largeur + gap
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

  updateGalleryVisibility();

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

  updateArrows();

  // Si tu as un système d'ajout d'images, tu dois appeler updateGalleryVisibility() et updateArrows() après ajout/suppression.
  const fileInput = document.getElementById("fileInput");

    fileInput.addEventListener('change', async (event) => {
    const file = event.target.files[0];
    if (!file) return;

    // Préparer les données à envoyer
    const formData = new FormData();
    formData.append('image', file);

    try {
        // ⬆️ Envoie l'image au serveur
        const response = await fetch('/upload', {
        method: 'POST',
        body: formData,
        });

        if (!response.ok) {
        throw new Error('Erreur lors de l\'upload');
        }

        // ✅ Lecture de la réponse du serveur
        const imageUrl = await response.text(); // ou JSON si tu préfères
        console.log('Image enregistrée:', imageUrl);

        // ⬇️ Crée et insère la nouvelle image dans la galerie
        const newImageBox = document.createElement('div');
        newImageBox.classList.add('image-box');

        const img = document.createElement('img');
        img.src = imageUrl;
        img.alt = file.name;

        newImageBox.appendChild(img);

        galleryContainer.insertBefore(newImageBox, addBox);

        // Reset input pour permettre un nouvel upload
        fileInput.value = '';

        // ✅ Met à jour la visibilité et les flèches
        updateGalleryVisibility();
        updateArrows();
    } catch (error) {
        console.error('Upload échoué:', error);
        alert('Échec de l\'upload : ' + error.message);
    }
    });
});

async function loadPortfolioData() {
    try {
        const response = await fetch('portfolio.json');
        
        if (!response.ok) {
            throw new Error(`Erreur HTTP: ${response.status}`);
        }
        
        const data = await response.json();
        console.log('Données chargées:', data);
        
        // Mettre à jour le contenu de la page
        updatePersonalInfo(data.personal);
        displaySkills(data.skills);
        showProjects(data.projects);
    } catch (error) {
        console.error('Erreur lors du chargement des données:', error);
        // Afficher l'erreur dans un conteneur existant
        const skillsContainer = document.getElementById('skills-container');
        if (skillsContainer) {
            skillsContainer.innerHTML = `<p style="color: red;">Erreur: ${error.message}</p>`;
        }
    }
}

function updatePersonalInfo(personal) {
    if (!personal) {
        console.warn('Données personal manquantes');
        return;
    }
    
    const h1 = document.querySelector('h1');
    if (h1) {
        h1.textContent = `Welcome to ${personal.name}'s Portfolio`;
    }
}

function displaySkills(skills) {
    // UTILISER le conteneur existant au lieu de créer skillsSection
    const skillsContainer = document.getElementById('skills-container');
    if (!skillsContainer) {
        console.error('Conteneur skills-container non trouvé');
        return;
    }
    
    if (!skills || !Array.isArray(skills)) {
        console.warn('Données skills manquantes ou invalides');
        skillsContainer.innerHTML = '<p>Aucune compétence à afficher</p>';
        return;
    }
    
    // Vider et remplir le conteneur
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
    // UTILISER le conteneur existant au lieu de créer projectsSection
    const projectsContainer = document.getElementById('projects-container');
    if (!projectsContainer) {
        console.error('Conteneur projects-container non trouvé');
        return;
    }
    
    if (!projects || !Array.isArray(projects)) {
        console.warn('Données projects manquantes ou invalides');
        projectsContainer.innerHTML = '<p>Aucun projet à afficher</p>';
        return;
    }
    
    // Vider et remplir le conteneur
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

// Charger les données au chargement de la page
document.addEventListener('DOMContentLoaded', loadPortfolioData);