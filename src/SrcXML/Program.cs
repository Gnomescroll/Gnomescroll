using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Text.RegularExpressions;

namespace SrcXML
{
    class Program
    {
        static void Main(string[] args)
        {
            if (!File.Exists("C:/dc_mmo/src/SrcXML/Config.txt"))
            {
                ERROR("Config.txt can't be found");
            }
            Console.WriteLine("Scanning Config.txt");
            List<string> Folders = new List<string>();
            List<string> Projects = new List<string>();
            string FOLDERPATH = "";
            string[] LINES = File.ReadAllLines("C:/dc_mmo/src/SrcXML/Config.txt");
            string FOLDERSREX = "";
            foreach (string LINE in LINES)
            {
                if (LINE.Split('=').Length > 1)
                {
                    string KEY = LINE.Split('=')[0];
                    string VALUE = LINE.Split('=')[1];
                    foreach (string sVALUE in VALUE.Split(';'))
                    {
                        switch (KEY.Trim().ToLower())
                        {
                            case "folderpath":
                                {
                                    FOLDERPATH = sVALUE.Trim();
                                    Console.WriteLine("Folder path: " + sVALUE.Trim());
                                }
                                break;
                            case "folders":
                                {
                                    FOLDERSREX = sVALUE.Trim().Replace(';', '|');
                                    Folders.Add(sVALUE.Trim());
                                    Console.WriteLine("Folder added: " + sVALUE.Trim());
                                }
                                break;
                            case "projects":
                                {
                                    Projects.Add(sVALUE.Trim());
                                    Console.WriteLine("Project added: " + sVALUE.Trim());
                                }
                                break;
                        }
                    }
                }
            }
            List<string> CFiles = new List<string>();
            List<string> HFiles = new List<string>();
            List<string> NFiles = new List<string>();
            foreach (string FOLDER in Folders)
            {
                Console.WriteLine("START Scanning folder: " + FOLDERPATH + FOLDER);
                string[] FILES = GetAllFiles(FOLDERPATH + FOLDER);
                foreach (string FILE in FILES)
                {
                    string EXTENSION = Path.GetExtension(FILE).ToLower();
                    string FILERES = FILE.Replace(FOLDERPATH, "");
                    switch (EXTENSION)
                    {
                        case ".c":
                        case ".cpp":
                            {
                                CFiles.Add(FILERES);
                            }
                            break;
                        case ".hpp":
                            {
                                HFiles.Add(FILERES);
                            }
                            break;
                        default:
                            {
                                NFiles.Add(FILERES);
                            }
                            break;
                    }
                }
                Console.WriteLine("END Scanning folder: " + FOLDERPATH + FOLDER);
            }
            Console.WriteLine("Found '" + CFiles.Count + "' C/C++ files");
            Console.WriteLine("Found '" + HFiles.Count + "' Header files");
            Console.WriteLine("Found '" + NFiles.Count + "' Unknown files");

            StringBuilder CPPFiles = new StringBuilder();
            {
                CPPFiles.AppendLine("\t<ItemGroup>");
                foreach (string FILE in CFiles)
                {
                    CPPFiles.AppendLine("\t\t<ClInclude Include=\"..\\" + FILE + "\" />");
                }
                CPPFiles.AppendLine("\t</ItemGroup>");
            }
            Console.WriteLine("Built XML for C/C++ files");

            StringBuilder HEADFiles = new StringBuilder();
            {
                HEADFiles.AppendLine("\t<ItemGroup>");
                foreach (string FILE in HFiles)
                {
                    HEADFiles.AppendLine("\t\t<ClInclude Include=\"..\\" + FILE + "\" />");
                }
                HEADFiles.AppendLine("\t</ItemGroup>");
            }
            Console.WriteLine("Built XML for Header files");

            StringBuilder UFiles = new StringBuilder();
            {
                UFiles.AppendLine("\t<ItemGroup>");
                foreach (string FILE in NFiles)
                {
                    UFiles.AppendLine("\t\t<ClInclude Include=\"..\\" + FILE + "\" />");
                }
                UFiles.AppendLine("\t</ItemGroup>");
            }
            Console.WriteLine("Built XML for Unknown files");

            Console.WriteLine("Building filter stuff");
            StringBuilder FilterFile = new StringBuilder();
            FilterFile.AppendLine("<ItemGroup>");
            Dictionary<string, bool> FILTERDIRS = new Dictionary<string, bool>();
            List<string> AllFiles = new List<string>();
            foreach (string FILE in CFiles)
            {
                AllFiles.Add(FILE);
            }
            foreach (string FILE in HFiles)
            {
                AllFiles.Add(FILE);
            }
            foreach (string FILE in NFiles)
            {
                AllFiles.Add(FILE);
            }
            foreach (string FILE in AllFiles)
            {
                string FILEPATH = FILE.Replace("\\" + Path.GetFileName(FILE), "");
                if (!FILTERDIRS.ContainsKey(FILEPATH))
                {
                    FILTERDIRS.Add(FILEPATH, true);
                }
                string[] SPLITPATH = FILEPATH.Split('\\');
                string SDFULL = "";
                foreach (string SD in SPLITPATH)
                {
                    SDFULL += "\\" + SD;
                    while (SDFULL[0] == '\\')
                    {
                        SDFULL = SDFULL.Substring(1);
                    }
                    if (!FILTERDIRS.ContainsKey(SDFULL))
                    {
                        FILTERDIRS.Add(SDFULL, true);
                    }
                }
                FilterFile.AppendLine("\t<ClInclude Include=\"..\\" + FILE + "\">");
                FilterFile.AppendLine("\t\t<Filter>" + FILEPATH + "</Filter>");
                FilterFile.AppendLine("\t</ClInclude>");
            }
            FilterFile.AppendLine("</ItemGroup>");

            StringBuilder FilterFileHeader = new StringBuilder();
            FilterFileHeader.AppendLine("\t<ItemGroup>");
            foreach (KeyValuePair<string, bool> FILTERX in FILTERDIRS)
            {
                FilterFileHeader.AppendLine("\t\t<Filter Include=\"" + FILTERX.Key + "\">");
                FilterFileHeader.AppendLine("\t\t</Filter>");
            }
            FilterFileHeader.AppendLine("\t</ItemGroup>");
            Console.WriteLine("Finished building filter stuff");

            foreach (string SD in Projects)
            {
                string SDFilters = SD + ".filters";
                // Project
                {
                    Console.WriteLine("Replacing project stuff from: " + SD);
                    string FinalXML = CPPFiles.ToString() + "\n" + HEADFiles.ToString() + "\n" + UFiles.ToString() + "\n";
                    foreach (string FSD in Folders)
                    {
                        string ProjectXMLFile = File.ReadAllText(SD);
                        string ResultString = null;
                        try
                        {
                            ResultString = Regex.Replace(
                                                            ProjectXMLFile,
                                                            "<ItemGroup>(?!.*stdafx).*?(" + FOLDERSREX + ").*?</ItemGroup>",
                                                            new MatchEvaluator(ComputeReplacement),
                                                            RegexOptions.Singleline | RegexOptions.IgnoreCase
                                                       );
                            ProjectXMLFile = ResultString;
                        }
                        catch { }
                        ProjectXMLFile = ProjectXMLFile.Replace(@"<Import Project=""$(VCTargetsPath)\Microsoft.Cpp.targets"" />", FinalXML + "\n" + @"<Import Project=""$(VCTargetsPath)\Microsoft.Cpp.targets"" />");
                        File.WriteAllText(SD, ProjectXMLFile);
                    }
                }
                // Filter
                {
                    Console.WriteLine("Replacing filter file: " + SDFilters);
                    string FinalFilterXML = FilterFileHeader.ToString() + "\n" + FilterFile.ToString() + "\n";

                    string ProjectXMLFile = File.ReadAllText(SDFilters);
                    string ResultString = null;
                    try
                    {
                        ResultString = Regex.Replace(
                                                        ProjectXMLFile,
                                                        "<ItemGroup>(?!.*stdafx).*?(" + FOLDERSREX + ").*?</ItemGroup>",
                                                        new MatchEvaluator(ComputeReplacement),
                                                        RegexOptions.Singleline | RegexOptions.IgnoreCase
                                                   );
                        ProjectXMLFile = ResultString;
                    }
                    catch { }
                    ProjectXMLFile = ProjectXMLFile.Replace("</Project>", FinalFilterXML + "\n</Project>");
                    File.WriteAllText(SDFilters, ProjectXMLFile);
                }
            }
            Console.WriteLine("All done... Press any key to continue....");
            Console.ReadKey();
        }
        static string[] GetAllFiles(string PATH)
        {
            string[] FILES = Directory.GetFiles(PATH, "*.*", SearchOption.AllDirectories);
            return FILES;

        }
        static void ERROR(string msg)
        {
            Console.WriteLine("An Error occured: " + msg + "\n");
            Console.WriteLine("Press any key to continue.\n");
            Console.ReadKey();
            Environment.Exit(1);
        }
        public static String ComputeReplacement(Match m)
        {
            // You can vary the replacement text for each match on-the-fly
            return "";
        }
    }
}
