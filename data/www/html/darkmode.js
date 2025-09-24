document.addEventListener("DOMContentLoaded", function () {
  const toggle = document.getElementById("darkModeToggle");

  // Appliquer le mode sombre si activé précédemment
  if (localStorage.getItem("darkMode") === "enabled") {
    document.body.classList.add("dark-mode");
    toggle.checked = true;
  }

  toggle.addEventListener("change", function () {
    const isDark = toggle.checked;

    document.body.classList.toggle("dark-mode", isDark);
    localStorage.setItem("darkMode", isDark ? "enabled" : "disabled");
  });
});