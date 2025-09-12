async function loadPortfolioData() {
    try {
        const response = await fetch('/portfolio.json');
        const data = await response.json();
        
        // Mettre à jour le contenu de la page
        updatePersonalInfo(data.personal);
        displaySkills(data.skills);
        showProjects(data.projects);
    } catch (error) {
        console.error('Erreur lors du chargement des données:', error);
    }
}

function updatePersonalInfo(personal) {
    document.querySelector('h1').textContent = `Welcome to ${personal.name}'s Portfolio`;
    // Ajouter d'autres mises à jour...
}

function displaySkills(skills) {
    // Créer une section skills dynamique
    const skillsSection = document.createElement('div');
    skillsSection.className = 'skills-section';
    // Ajouter le contenu...
}

// Charger les données au chargement de la page
document.addEventListener('DOMContentLoaded', loadPortfolioData);